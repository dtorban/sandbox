

#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0, set = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(binding = 0, set = 1) uniform UniformBufferObject2 {
    mat4 transform;
} ubo2;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 3) in vec3 location;
layout(location = 4) in vec3 info;
layout(location = 5) in vec4 armAngles[4];
layout(location = 9) in vec4 armLengths[4];
//layout(location = 4) in vec3 numArms
//layout(location = 5) in mat4 armAngles
//layout(location = 9) in mat4 armAngles2
//layout(location = 13) in mat4 armLengths
//layout(location = 17) in mat4 armLengths2
//layout(location = 21) in mat4 value
//layout(location = 25) in mat4 value2


layout(location = 0) out vec3 fragNorm;
layout(location = 1) out vec2 fragTexCoord;

float calculateLength(float angle, vec3 pos, int prevArm, int curArm, float oldLen) {
	float curAngle = armAngles[curArm/4][curArm - (curArm/4)*4];;
	float prevAngle = armAngles[prevArm/4][prevArm - (prevArm/4)*4];
	if (prevArm > curArm) {
		if (angle < curAngle) {
			angle += 2.0*3.1459;
		}
		curAngle += 2.0*3.1459;
	}

	//float curLength = length(pos.xy)*armLengths[curArm/4][curArm - (curArm/4)*4];
	//float prevLength = length(pos.xy)*armLengths[prevArm/4][prevArm - (prevArm/4)*4];
	float curLength = armLengths[curArm/4][curArm - (curArm/4)*4];
	float prevLength = armLengths[prevArm/4][prevArm - (prevArm/4)*4];

	float sectionAngle = curAngle-prevAngle;
	if (sectionAngle >= 3.14159) {
		return oldLen;
	}

	float lerp = (angle-prevAngle)/sectionAngle;
	//float interp = (1.0-sin(lerp*3.14159))/2.0 + 0.5;
	float stretchFactor = ((1.0-.1) + 0.5)*sectionAngle/(3.14159);
	float interp = pow((lerp-0.5)*2.0,2.0)*stretchFactor + 1.0-stretchFactor;

	float len = interp*((1.0-lerp)*prevLength+lerp*curLength);
	if (oldLen > len) {
		len = oldLen;
	}
	return len;
}

float cross2(vec2 a, vec2 b) {
	return a.x*b.y-a.y*b.x;
}

float calculateLength2(float angle, vec3 pos, int prevArm, int curArm, float oldLen) {
	float curLength = armLengths[curArm/4][curArm - (curArm/4)*4];
	float prevLength = armLengths[prevArm/4][prevArm - (prevArm/4)*4];
	float curAngle = armAngles[curArm/4][curArm - (curArm/4)*4];;
	float prevAngle = armAngles[prevArm/4][prevArm - (prevArm/4)*4];

	if (prevArm > curArm) {
		if (angle < curAngle) {
			angle += 2.0*3.1459;
		}
		curAngle += 2.0*3.1459;
	}

	float sectionAngle = curAngle-prevAngle;
	if (sectionAngle >= 3.14159) {
		return oldLen;
	}

	vec2 curPos = vec2(curLength*cos(curAngle), curLength*sin(curAngle));
	vec2 prevPos = vec2(prevLength*cos(prevAngle), prevLength*sin(prevAngle));

	vec2 q = prevPos;
	vec2 s = curPos-prevPos;
	vec2 r = vec2(cos(angle), sin(angle));
	float t = cross2(q,s)/cross2(r,s);
	float len = t;
	vec2 newPos = t*r;
	float interp = length(newPos-q)/length(s);

	float dist = (angle-prevAngle)/sectionAngle;

	//len = len * pow((dist-0.5)*2.0,2.0)/2.0+0.5;
	//len = len * abs(dist-0.5)*2.0;

	/*vec2 interpP = (1.0-interp)*prevPos;
	vec2 interpC = interp*(curPos);
	newPos = interpP + interp*(interpC-interpP);
	//vec2 interpPoint = interp*(curPos) + (1.0-interp)*prevPos;
	len = length(newPos);*/

	vec2 midpoint = (prevPos+curPos)/4.0;
	vec2 lineMidpoint = (prevPos + 0.5*s);
	vec2 sinDir = midpoint-lineMidpoint;
	//newPos = newPos+sin(interp*3.14159)*sinDir;
	//len = length(newPos);

	
	//len = pow((interp-0.5)*2.0,2.0)/2.0+0.5/len;
	float stretchFactor = 3.0*(1.0+1.0-sectionAngle/3.14159);
	float extra = (interp*curLength + (1-interp)*prevLength - t);
	//extra = pow(extra,stretchFactor/2.0);
	//extra = abs((interp-0.5)*2.0)/2.0+0.5;
	len = t-extra;
	//stretchFactor = t/(interp*curLength + (1-interp)*prevLength);
	//stretchFactor = pow(2,stretchFactor);//*stretchFactor;
	len = t*pow(t/(interp*curLength + (1-interp)*prevLength),stretchFactor);

	//len = (pow((dist-0.5)*2.0,2.0)/2.0+0.5)*t/len;

	if (oldLen > len) {
		len = oldLen;
	}

	return len;
}

void main() {
	vec3 pos = inPosition;
	float angle = atan(pos.y, pos.x)+3.14159;
	int numArms = int(info.x);

	int prevArm = 0;
	int curArm = 0;
	int step = 1;
	for (int f = 0; f < 16; f++) {
		if (f < numArms) {
			float newAngle = armAngles[f/4][f - (f/4)*4];

			if (newAngle < angle) {
				curArm = ((f+1)+numArms)-(((f+step)+numArms)/numArms)*numArms;
				prevArm = f;
			}

			if (f == 0 && newAngle > angle) {
				curArm = 0;
				prevArm = numArms-1;
			}
		}
	}

	
	float len = calculateLength(angle, pos, prevArm, curArm, 0.0);
	len = calculateLength(angle, pos, ((prevArm-1)+numArms)-(((prevArm-1)+numArms)/numArms)*numArms, curArm, len);
	len = calculateLength(angle, pos, prevArm, ((curArm+1)+numArms)-(((curArm+1)+numArms)/numArms)*numArms, len);
	len = calculateLength(angle, pos, ((prevArm-2)+numArms)-(((prevArm-2)+numArms)/numArms)*numArms, curArm, len);
	len = calculateLength(angle, pos, prevArm, ((curArm+2)+numArms)-(((curArm+2)+numArms)/numArms)*numArms, len);

	len = calculateLength2(angle, pos, prevArm, curArm, 0.0);
	len = calculateLength2(angle, pos, ((prevArm-1)+numArms)-(((prevArm-1)+numArms)/numArms)*numArms, curArm, len);
	len = calculateLength2(angle, pos, prevArm, ((curArm+1)+numArms)-(((curArm+1)+numArms)/numArms)*numArms, len);
	len = calculateLength2(angle, pos, ((prevArm-2)+numArms)-(((prevArm-2)+numArms)/numArms)*numArms, curArm, len);
	len = calculateLength2(angle, pos, prevArm, ((curArm+2)+numArms)-(((curArm+2)+numArms)/numArms)*numArms, len);

	len = length(pos.xy)*len;

	fragNorm = inNormal;

	pos.x = len*cos(angle);
	pos.y = len*sin(angle);
	if (pos.z < 0) {
		pos.z /= 10.0;
    	fragNorm.z *= 10;
	}
	else {
		pos.z /= 5.0;
    	fragNorm.z *= 5;
	}
	pos.z += 1.0/10.0/2.0;
	pos += location;
    gl_Position = ubo.proj * ubo.view * ubo.model * ubo2.transform * vec4(pos, 1.0);
    fragTexCoord = inTexCoord;
    
    //fragNorm = normalize(pos);
}

