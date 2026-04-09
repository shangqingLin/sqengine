#pragma once

namespace phxy
{

    struct SqColor
    {
        SqColor() {}
        SqColor(float rIn, float gIn, float bIn, float aIn = 1.0f)
        {
            r = rIn;
            g = gIn;
            b = bIn;
            a = aIn;
        }

        void Set(float rIn, float gIn, float bIn, float aIn = 1.0f)
        {
            r = rIn;
            g = gIn;
            b = bIn;
            a = aIn;
        }

        float r, g, b, a;
    };

    class SqParticleColor
    {
    public:
        SqParticleColor() {
            r = g = b = a = 0;
        }
        /// Constructor with four elements: r (red), g (green), b (blue), and a
        /// (opacity).
        /// Each element can be specified 0 to 255.
        inline SqParticleColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
        {
            Set(r, g, b, a);
        }

        /// Constructor that initializes the above four elements with the value of
        /// the SqColor object.
        SqParticleColor(const SqColor &color);

        /// True when all four color elements equal 0. When true, a particle color
        /// buffer isn't allocated by CreateParticle().
        ///
        bool IsZero() const
        {
            return !r && !g && !b && !a;
        }

        /// Used internally to convert the value of SqColor.
        ///
        SqColor GetColor() const;

        /// Sets color for current object using the four elements described above.
        ///
        inline void Set(unsigned char r_, unsigned char g_, unsigned char b_, unsigned char a_)
        {
            r = r_;
            g = g_;
            b = b_;
            a = a_;
        }

        /// Initializes the object with the value of the SqColor.
        ///
        void Set(const SqColor &color);

        /// Assign a SqParticleColor to this instance.
        SqParticleColor &operator=(const SqParticleColor &color)
        {
            Set(color.r, color.g, color.b, color.a);
            return *this;
        }

        /// Multiplies r, g, b, a members by s where s is a value between 0.0
        /// and 1.0.
        SqParticleColor &operator*=(float s)
        {
            Set((unsigned char)(r * s), (unsigned char)(g * s), (unsigned char)(b * s), (unsigned char)(a * s));
            return *this;
        }

        /// Scales r, g, b, a members by s where s is a value between 0 and 255.
        SqParticleColor &operator*=(unsigned char s)
        {
            // 1..256 to maintain the complete dynamic range.
            const int scale = (int)s + 1;
            Set((unsigned char)(((int)r * scale) >> k_bitsPerComponent),
                (unsigned char)(((int)g * scale) >> k_bitsPerComponent),
                (unsigned char)(((int)b * scale) >> k_bitsPerComponent),
                (unsigned char)(((int)a * scale) >> k_bitsPerComponent));
            return *this;
        }

        /// Scales r, g, b, a members by s returning the modified SqParticleColor.
        SqParticleColor operator*(float s) const
        {
            return MultiplyByScalar(s);
        }

        /// Scales r, g, b, a members by s returning the modified SqParticleColor.
        SqParticleColor operator*(unsigned char s) const
        {
            return MultiplyByScalar(s);
        }

        /// Add two colors.  This is a non-saturating addition so values
        /// overflows will wrap.
        inline SqParticleColor &operator+=(const SqParticleColor &color)
        {
            r += color.r;
            g += color.g;
            b += color.b;
            a += color.a;
            return *this;
        }

        /// Add two colors.  This is a non-saturating addition so values
        /// overflows will wrap.
        SqParticleColor operator+(const SqParticleColor &color) const
        {
            SqParticleColor newColor(*this);
            newColor += color;
            return newColor;
        }

        /// Subtract a color from this color.  This is a subtraction without
        /// saturation so underflows will wrap.
        inline SqParticleColor &operator-=(const SqParticleColor &color)
        {
            r -= color.r;
            g -= color.g;
            b -= color.b;
            a -= color.a;
            return *this;
        }

        /// Subtract a color from this color returning the result.  This is a
        /// subtraction without saturation so underflows will wrap.
        SqParticleColor operator-(const SqParticleColor &color) const
        {
            SqParticleColor newColor(*this);
            newColor -= color;
            return newColor;
        }

        /// Compare this color with the specified color.
        bool operator==(const SqParticleColor &color) const
        {
            return r == color.r && g == color.g && b == color.b && a == color.a;
        }

        /// Mix mixColor with this color using strength to control how much of
        /// mixColor is mixed with this color and vice versa.  The range of
        /// strength is 0..128 where 0 results in no color mixing and 128 results
        /// in an equal mix of both colors.  strength 0..128 is analogous to an
        /// alpha channel value between 0.0f..0.5f.
        inline void Mix(SqParticleColor *const mixColor, const int strength)
        {
            MixColors(this, mixColor, strength);
        }

        /// Mix colorA with colorB using strength to control how much of
        /// colorA is mixed with colorB and vice versa.  The range of
        /// strength is 0..128 where 0 results in no color mixing and 128 results
        /// in an equal mix of both colors.  strength 0..128 is analogous to an
        /// alpha channel value between 0.0f..0.5f.
        static inline void MixColors(SqParticleColor *const colorA,
                                     SqParticleColor *const colorB,
                                     const int strength)
        {
            const unsigned char dr = (unsigned char)((strength * (colorB->r - colorA->r)) >>
                                                     k_bitsPerComponent);
            const unsigned char dg = (unsigned char)((strength * (colorB->g - colorA->g)) >>
                                                     k_bitsPerComponent);
            const unsigned char db = (unsigned char)((strength * (colorB->b - colorA->b)) >>
                                                     k_bitsPerComponent);
            const unsigned char da = (unsigned char)((strength * (colorB->a - colorA->a)) >>
                                                     k_bitsPerComponent);
            colorA->r += dr;
            colorA->g += dg;
            colorA->b += db;
            colorA->a += da;
            colorB->r -= dr;
            colorB->g -= dg;
            colorB->b -= db;
            colorB->a -= da;
        }

    private:
        /// Generalization of the multiply operator using a scalar in-place
        /// multiplication.
        template <typename T>
        SqParticleColor MultiplyByScalar(T s) const
        {
            SqParticleColor color(*this);
            color *= s;
            return color;
        }

    public:
        unsigned char r, g, b, a;

    protected:
        /// Maximum value of a SqParticleColor component.
        static const float k_maxValue;
        /// 1.0 / k_maxValue.
        static const float k_inverseMaxValue;
        /// Number of bits used to store each SqParticleColor component.
        static const unsigned char k_bitsPerComponent;
    };
}