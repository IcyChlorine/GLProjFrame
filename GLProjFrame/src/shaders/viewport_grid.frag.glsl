#version 330 core

in vec3 pos;
in vec4 pos_clip;

out vec4 FragColor;

uniform float zNear;
uniform float zFar;


/*
 * Notes on linearizing depth:
 * 
 * Under conventional graphics pipeline, there're several quantities related to depth,
 * which is quite confusing. Next, I first list their definition, before exploring 
 * possible ways to generate linearized depth from them.
 *
 * 1. vec4 gl_Position, which is essentially v_clip(goto the link below for definition of these coordinate systems).
 *    we have z_clip in [-1, 1], w_clip = -z_eye.
 * 2. vec3 NDC, defined as v_ndc = v_clip.xyz / v_clip.w.
 * 3. gl_FragCoord, which is a somewhat "window" coordinate derived from mapping v_ndc in the following way:
 *    gl_FragCoord.x = (x_ndc linear mapped from [-1,1] to [0, width-1])
 *    gl_FragCoord.y = (y_ndc linear mapped from [-1,1] to [0, height-1])
 *    gl_FragCoord.z = (z_ndc linear mapped from [-1,1] to [0, 1])
 *    gl_FragCoord.w = (1/w_clip)
 *
 * Among 1, 2, and 3, gl_Position.zw, NDC.z, and gl_FragCoord.zx are related to depth, most of them are non-linear.
 * And note that, only gl_Position(by passing data from vertex shader to fragment shader) and gl_FragCoord are
 * accessible in fragment shader.
 *
 * What we want as "linear depth", is a quantity linear to the Euclidean distance between the fragment and the camera.
 * What we want is (-z_eye). But as z_eye is in [zNear, zFar] and can be hard to visualize, we want to map it to [0, 1].
 *
 * Examining gl_Position(v_clip) and gl_FragCoord(v_ndc and w_clip), we have several ways to derive it.
 * 1. (require passing v_clip) z_clip = A*z_eye+B => z_eye = (z_clip-B)/A. To implement:
 *    float z_eye_n = (z_clip*(zFar-zNear)+2*zFar*zNear) / (zFar+zNear);
 * 2. (require passing v_clip) z_eye = -w_clip. This is actually the easiest formula of all! To implement:
 *    float z_eye_n = w_clip;
 * 3. Get z_eye from gl_FragCoord
 *    (Step 1) z_ndc = 2*gl_FragCoord.z-1.
 *    (Step 2) z_eye = (z_ndc*(zFar-zNear)-2*zFar*zNear) / (zFar+zNear). To implement:
 *     float z_eye_n = (2.0 * zNear * zFar) / (zFar + zNear + z_ndc * (zNear - zFar)); (LearnOpenGL's method)
 *    Or:  z_clip = z_ndc * w_clip , and go back to method 1 or 2. (this is a bit stupid...)
 * 4. Exploit gl_FragCoord.w = 1/w_clip, and w_clip = -z_eye, we have z_eye = -1/gl_FragCoord.w.
 *    - this should be the most computationally efficient way, given no extra data are passed between vert and frag.
 *   
 * Finally, map z_eye to [0, 1] by (|z_eye|-zNear)/(zFar-zNear) if necessary.
 *
 *
 * (see https://www.songho.ca/opengl/gl_projectionmatrix.html for definitions of different coordinate systems)
 */

float linearize_depth(float z_clip)
{
    // Get z_ndc
    //float z_ndc = gl_FragCoord.z*2-1;
    // or
    float z_ndc = pos_clip.z/pos_clip.w;

    // (negative) z_eye, see https://www.songho.ca/opengl/gl_projectionmatrix.html
    // z_eyen will be in [zNear, zFar]
    float z_eye_n;
    
    // z_clip to z_eye
    //z_eye_n = (z_clip*(zFar-zNear)+2*zFar*zNear) / (zFar+zNear);
    // z_ndc to z_eye
    z_eye_n = (2.0 * zNear * zFar) / (zFar + zNear - z_ndc * (zFar - zNear));

    // map to [0, 1]
    //return (z_eye_n-zNear)/(zFar-zNear);


    // or, realizing that zNear is usually small, use the following (appxoximation) suite instead:
    // z_clip to z_eye
    z_eye_n = (z_clip*(zFar)+2*zFar*zNear) / zFar;
    // z_ndc to z_eye
    //z_eye_n = (2.0 * zNear * zFar) / (zFar - z_ndc * zFar); // Note: the denominator of this formula is not quite stable... So this approx is poor
    return z_eye_n / zFar;
}

// linear depth magic, using the most computationally efficient method
float ldm() {
    // return (1/gl_FragCoord.w-zNear)/(zFar-zNear);
    // approximate as zNear is usually vanishingly small
    return 1/(gl_FragCoord.w*zFar);
}

void main()
{
    vec3 n = round(pos);
    if(abs(pos.x-n.x)<0.05 || abs(pos.z-n.z)<0.05)
        FragColor = vec4(vec3(1.0), 0.5); 
    else 
        discard;

    FragColor = vec4(vec3(ldm()), 1.0);
}