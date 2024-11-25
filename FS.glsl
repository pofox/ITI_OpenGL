#version 330
 
in vec4 frag_color;
 
#define MAX_ITERATIONS 50

uniform float theta;
 
int get_iterations()
{
    float real = frag_color.x*2*theta;
    float imag = frag_color.y*2*theta;
 
    int iterations = 0;
    float const_real = real;
    float const_imag = imag;
 
    while (iterations < MAX_ITERATIONS)
    {
        float tmp_real = real;
        real = (real * real - imag * imag) + const_real;
        imag = (2.0 * tmp_real * imag) + const_imag;
         
        float dist = real * real + imag * imag;
         
        if (dist > 2.0)
        break;
 
        ++iterations;
    }
    return iterations;
}

vec4 return_color()
{
    int iter = get_iterations();
    if (iter == MAX_ITERATIONS)
    {
        //gl_FragDepth = 0.0f;
        return vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }
 
    float iterations = float(iter*50) / MAX_ITERATIONS;    
    return vec4((cos(sqrt(iterations)))*(cos(sqrt(iterations))), (cos(sqrt(iterations)+120.f/255))*(cos(sqrt(iterations)+120.f/255)), (cos(sqrt(iterations)+240.f/255))*(cos(sqrt(iterations)+240.f/255)), 1.0f);
}
 
void main()
{
    gl_FragColor = return_color();
}