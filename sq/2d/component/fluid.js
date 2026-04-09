
//https://jamie-wong.com/2016/08/05/webgl-fluid-simulation/
(function () {

    var canvas = document.getElementById('c'),
        WIDTH = canvas.width,
        HEIGHT = canvas.height,
        sx = canvas.width / canvas.clientWidth,
        sy = canvas.height / canvas.clientHeight,
        ctx = canvas.getContext('2d');

    var mouseX = 0, mouseY = 0;
    (function () {
        var rect = canvas.getBoundingClientRect(),
            left = rect.left,
            top = rect.top;
        canvas.addEventListener('mousemove', function (e) {
            mouseX = (e.clientX - left) | 0,
            mouseY = (e.clientY - top) | 0;
        });
    })();



    ctx.fillRect(0, 0, WIDTH, HEIGHT);

    var imageData = ctx.getImageData(0, 0, WIDTH, HEIGHT);

    //速度场
    var velocityField0 = new Float32Array(WIDTH * HEIGHT * 2);
    var u0x = sampler(velocityField0, WIDTH, HEIGHT, 2, 0);
    var u0y = sampler(velocityField0, WIDTH, HEIGHT, 2, 1);

    var velocityField1 = new Float32Array(WIDTH * HEIGHT * 2);
    var u1x = sampler(velocityField1, WIDTH, HEIGHT, 2, 0);
    var u1y = sampler(velocityField1, WIDTH, HEIGHT, 2, 1);

    //压力
    var pressureField0 = new Float32Array(WIDTH * HEIGHT);
    var p0 = sampler(pressureField0, WIDTH, HEIGHT, 1, 0);
      
    var pressureField1 = new Float32Array(WIDTH * HEIGHT);
    var p1 = sampler(pressureField1, WIDTH, HEIGHT, 1, 0);

    //发散
    var divergenceField = new Float32Array(WIDTH * HEIGHT);
    var div = sampler(divergenceField, WIDTH, HEIGHT, 1, 0);
    var step = 4.0;

    for (var i = 0; i < pressureField0.length; i++) {
        pressureField0[i] = 0;
        pressureField1[i] = pressureField0[i];
    }

    for (i = 0; i < velocityField0.length; i++) {
        // velocityField0[i] = (Math.random()-0.5)*10.0;
        velocityField1[i] = velocityField0[i];
    }

    velocityboundary(u0x, u0y);


    function simulate() {
        velocityboundary(u0x, u0y);
        
        advect(u0x, u0y, u0x, u1x, step); //x轴
        advect(u0x, u0y, u0y, u1y, step); //y轴

        addMouseForce(u1x, u1y);

        computeDivergence(u1x, u1y, div);

        // needs an even number of iterations
        fastjacobi(p0, p1, div, -1, 0.25, 16);
        //advect(u1x, u1y, p0, p1);
        //velocityField0 = diffuse(velocityField1, pressureField);
        //pressureField = recomputePressure(velocityField0);
        subtractPressureGradient(u1x, u1y, p0);
        var aux = p0;
        p0 = p1;
        p1 = aux;

        aux = u0x;
        u0x = u1x;
        u1x = aux;

        aux = u0y;
        u0y = u1y;
        u1y = aux;
    }
    var lastMouseX = mouseX,
        lastMouseY = mouseY;

    function addMouseForce(ux, uy) {
        var x = clamp(mouseX * sx, 1, WIDTH - 2),
            y = clamp(mouseY * sy, 1, HEIGHT - 2),
            dx = mouseX - lastMouseX,
            dy = mouseY - lastMouseY;
        lastMouseX = mouseX;
        lastMouseY = mouseY;
        ux(x, y, ux(x, y) - dx * 2);
        uy(x, y, uy(x, y) - dy * 2);
    }

    function pressureboundary(p) {
        for (var x = 0; x < WIDTH; x++) {
            p(x, 0, p(x, 1));
            p(x, HEIGHT - 1, p(x, HEIGHT - 2));
        }
        for (var y = 0; y < HEIGHT; y++) {
            p(0, y, p(1, y));
            p(WIDTH - 1, y, p(WIDTH - 2, y));
        }
    }

    function velocityboundary(ux, uy) {
        //对二维网格边缘的格子设置，将值设置为他的相邻行


        //顶部第一行设置为顶部第二行的值
        //底部第一行设置为底部第二行的值
        for (var x = 0; x < WIDTH; x++) {
            ux(x, 0, -ux(x, 1));
            uy(x, 0, -uy(x, 1));

            ux(x, HEIGHT - 1, -ux(x, HEIGHT - 2));
            uy(x, HEIGHT - 1, -uy(x, HEIGHT - 2));
        }

        //左边第一列设置左边第二例的值
        //右边第一列的值设置为右边第二列的值
        for (var y = 0; y < HEIGHT; y++) {
            ux(0, y, -ux(1, y));
            uy(0, y, -uy(1, y));

            ux(WIDTH - 1, y, -ux(WIDTH - 2, y));
            uy(WIDTH - 1, y, -uy(WIDTH - 2, y));
        }
    }

    function clamp(a, min, max) {
        return Math.max(Math.min(a, max), min);

    }

    function lerp(a, b, c) {
        c = c < 0 ? 0 : (c > 1 ? 1 : c);
        //c = clamp(c, 0, 1);
        return a * (1 - c) + b * c;
    }

    function sampler(a, width, height, stride, offset) {
        var f = function (x, y, value) {

            //保证x和y位置不超出width和height大小。 与0位运算保证x和y为正整数
            x = (x < 0 ? 0 : (x > width - 1 ? width - 1 : x)) | 0;
            y = (y < 0 ? 0 : (y > height - 1 ? height - 1 : y)) | 0;

            if (value !== undefined) {
                //将value填充到位置(x,y)上 。
                // x + y * width 这句代码将二维网格转换为格子的index。每个格子可能存储多个值，stride和offset就是控制读取格子上哪个值 
                a[(x + y * width) * stride + offset] = value;
            }
            else {
                //如果不指定value,则表示获取指定位置上的值
                return a[(x + y * width) * stride + offset];
            }
        };
        f.a = a;
        return f;
    }

    //双线性插值（Bilinear Interpolation）
    //获取各种周边四个格子，然后进行插值处理
    function bilerp(sample, x, y) {

        //~~x 是取整的技巧（等价于 Math.floor(x)，更快）。
        var x0 = ~~x,
            y0 = ~~y,
            x1 = x0 + 1,
            y1 = y0 + 1,
            p00 = sample(x0, y0), // 左上
            p01 = sample(x0, y1), // 左下
            p10 = sample(x1, y0), // 右上
            p11 = sample(x1, y1); // 右下
        return lerp(lerp(p00, p10, x - x0), lerp(p01, p11, x - x0), y - y0);
    }


    /**
     * 进行对流，即让速度流动起来
     * @param {*} ux 
     * @param {*} uy 
     * @param {*} src 
     * @param {*} dest 
     * @param {*} t 
     */
    function advect(ux, uy, src, dest, t) {
        for (var y = 1; y < HEIGHT - 1; y++) {
            for (var x = 1; x < WIDTH - 1; x++) {
                var vx = ux(x, y) * t,
                    vy = uy(x, y) * t; //速度乘以时间=position 新的位置
                dest(x, y, bilerp(src, x + vx, y + vy));
            }
        }
    }

    function computeDivergence(ux, uy, div) {
        for (var y = 1; y < HEIGHT - 1; y++) {
            for (var x = 1; x < WIDTH - 1; x++) {
                // compute divergence using central difference
                var x0 = ux(x - 1, y),
                    x1 = ux(x + 1, y),
                    y0 = uy(x, y - 1),
                    y1 = uy(x, y + 1);
                div(x, y, (x1 - x0 + y1 - y0) * 0.5);
            }
        }
    }
    // x = p
    // b = div
    function jacobi(p0, p1, b, alpha, beta, iterations) {
        for (var i = 0; i < pressureField0.length; i++) {
            pressureField0[i] = 0.5;
            pressureField1[i] = pressureField0[i];
        }

        for (i = 0; i < iterations; i++) {
            for (var y = 1; y < HEIGHT - 1; y++) {
                for (var x = 1; x < WIDTH - 1; x++) {
                    var x0 = p0(x - 1, y),
                        x1 = p0(x + 1, y),
                        y0 = p0(x, y - 1),
                        y1 = p0(x, y + 1);
                    p1(x, y, (x0 + x1 + y0 + y1 + alpha * b(x, y)) * beta);
                }
            }
            var aux = p0;
            p0 = p1;
            p1 = aux;
            pressureboundary(p0);
        }
    }

    function fastjacobi(p0, p1, b, alpha, beta, iterations) {
        p0 = p0.a;
        p1 = p1.a;
        b = b.a;
        //for(var i = 0; i < pressureField0.length; i++) {
        //pressureField0[i] = 0.5;
        //pressureField1[i] = pressureField0[i];
        //}

        for (i = 0; i < iterations; i++) {
            for (var y = 1; y < HEIGHT - 1; y++) {
                for (var x = 1; x < WIDTH - 1; x++) {
                    var pi = x + y * WIDTH,
                        x0 = p0[pi - 1],
                        x1 = p0[pi + 1],
                        y0 = p0[pi - WIDTH],
                        y1 = p0[pi + WIDTH];
                    p1[pi] = (x0 + x1 + y0 + y1 + alpha * b[pi]) * beta;
                }
            }
            var aux = p0;
            p0 = p1;
            p1 = aux;
            //pressureboundary(p0);
        }
    }

    function subtractPressureGradient(ux, uy, p) {
        for (var y = 1; y < HEIGHT - 1; y++) {
            for (var x = 1; x < WIDTH - 1; x++) {
                var x0 = p(x - 1, y),
                    x1 = p(x + 1, y),
                    y0 = p(x, y - 1),
                    y1 = p(x, y + 1),
                    dx = (x1 - x0) / 2,
                    dy = (y1 - y0) / 2;
                ux(x, y, ux(x, y) - dx);
                uy(x, y, uy(x, y) - dy);
            }
        }
    }

    function draw(ux, uy, p) {
        var d = imageData.data,
            di, pi, ui;
        for (var y = 0; y < HEIGHT; y++) {
            for (var x = 0; x < WIDTH; x++) {
                pi = (y * WIDTH + x);
                ui = pi * 2;
                di = pi * 4;
                d[di + 0] = p(x, y) * 555;
                d[di + 1] = ux(x, y) * 128 + 128;
                d[di + 2] = uy(x, y) * 128 + 128;
            }
        }
        ctx.putImageData(imageData, 0, 0);
    }


    var requestAnimationFrame = (window.requestAnimationFrame ||
        window.webkitRequestAnimationFrame ||
        window.mozRequestAnimationFrame ||
        window.oRequestAnimationFrame ||
        window.msRequestAnimationFrame ||
        function (callback) {
            window.setTimeout(callback, 1000 / 60);
        });

    (function animate() {
        simulate();
        draw(u0x, u0y, p0);
        requestAnimationFrame(animate);
    })();



})();

