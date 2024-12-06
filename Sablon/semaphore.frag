#version 330 core

in vec2 vTexCoord;
out vec4 FragColor;

uniform sampler2D uTex;
uniform sampler2D uArrowTex;  
uniform vec3 uLightStates;   
uniform float uArrowState;  

void main() {
    vec4 baseColor = vec4(0.0); 
    vec4 arrowColor = vec4(0.0);  

    if (vTexCoord.x <= 1.0) { 
        if (uLightStates.x > 0.5 && vTexCoord.x > 0.25 && vTexCoord.x <= 0.5) {
            baseColor = texture(uTex, vTexCoord);
        } else if (uLightStates.y > 0.5 && vTexCoord.x > 0.5 && vTexCoord.x <= 0.75) {
            baseColor = texture(uTex, vTexCoord); 
        } else if (uLightStates.z > 0.5 && vTexCoord.x > 0.75 && vTexCoord.x <= 1.0) {
            baseColor = texture(uTex, vTexCoord); 
        } else {
            baseColor = texture(uTex, vec2(mod(vTexCoord.x, 0.25), vTexCoord.y));
        }
    }
    
    if (vTexCoord.x > 1.0 && vTexCoord.x <= 1.5) { 
        vec2 arrowTexCoord = vec2((vTexCoord.x - 1.0), vTexCoord.y);

        if (uArrowState > 0.5) {
            arrowTexCoord.x += 0.5;
        }

        arrowColor = texture(uArrowTex, arrowTexCoord);
    }

    FragColor = arrowColor.a > 0.0 ? arrowColor : baseColor;
}
