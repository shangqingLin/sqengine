export const VertexMainWebGL1:string = `
void main(){
  gl_Position = vert();
}
`

export const VertexMainWebGL2:string = `
// out vec4 glPosition;  
void main(){
  gl_Position = vert();
}
`

export const FragMainWebGL1 = `
void main(){
  gl_FragColor = frag();
}
`

export const FragMainWebGL2 = `
out vec4 fragColor;
void main(){ fragColor=frag(); }
`
