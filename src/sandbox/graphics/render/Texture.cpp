#include "sandbox/graphics/render/Texture.h"
#include <iostream>
#include "sandbox/graphics/RenderState.h"

namespace sandbox {

Texture::Texture() : image(nullptr), textureVersion(-1), format(GL_RGBA), internalFormat(GL_RGBA) {
	addType<Texture>();
}

void Texture::update() {
	if (!image) {
		image = getEntity().getComponent<Image>();
		format = image->getComponents() == 4 ? GL_RGBA : GL_RGB;
		internalFormat = format;
		textureVersion++;
	}
}

void Texture::updateSharedContext(const GraphicsContext& context) {
	TextureSharedState& state = *contextHandler.getSharedState(context);

	if (image && !state.initialized) {
		state.textureVersion = textureVersion;
	    std::cout << "INitialize texture shared context " << std::endl;
	    GLuint format = getFormat();
		GLuint internalFormat = getInternalFormat();
		GLuint type = getType();

		state.target = GL_TEXTURE_2D;
		glGenTextures(1, &state.texture);
		glBindTexture(state.target, state.texture);
		glTexImage2D(state.target, 0, internalFormat, image->getWidth(), image->getHeight(), 0, format, type, image->getData());
	    state.initialized = true;
	}
	else if (!image && state.initialized) {
		glDeleteTextures(1, &state.texture);
	    state.initialized = false;
	}
	
	if (state.textureVersion != textureVersion) {
	    std::cout << "Update texture shared context " << std::endl;
		glBindTexture(state.target, state.texture);
		glTexImage2D(state.target, 0, getInternalFormat(), image->getWidth(), image->getHeight(), 0, getFormat(), getType(), image->getData());
		state.textureVersion = textureVersion;
	}
}

void Texture::updateContext(const GraphicsContext& context) {
	TextureSharedState& sharedState = *contextHandler.getSharedState(context);
	ContextState& state = *contextHandler.getState(context);

	if (image && !state.initialized) {
        std::cout << "INitialize texture context" << std::endl;
        glBindTexture(sharedState.target, sharedState.texture);
		glTexParameteri(sharedState.target, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(sharedState.target, GL_TEXTURE_MAX_LEVEL, 0);
		glTexParameteri(sharedState.target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(sharedState.target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(sharedState.target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(sharedState.target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(sharedState.target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	    state.initialized = true;
	}
	else if (!image && state.initialized) {
		state.initialized = false;
	}
}

GLuint Texture::getTarget(const GraphicsContext& context) {
	TextureSharedState& sharedState = *contextHandler.getSharedState(context);
	return sharedState.target;
}

GLuint Texture::getId(const GraphicsContext& context) {
	TextureSharedState& sharedState = *contextHandler.getSharedState(context);
	return sharedState.texture;

}

/*
void SBTexture::renderSpecific(Context& sharedContext, Context& context, SBTextureSharedState& sharedState, ContextState& state) const {
	glActiveTexture(GL_TEXTURE0+0);
	glBindTexture(sharedState.target, sharedState.texture);

	GLuint shaderProgram = ShaderProgram::getCurrentProgramId(sharedContext, context);
	GLint loc = glGetUniformLocation(shaderProgram, "tex");
	glUniform1i(loc, 0);

}
*/

void Texture::startRender(const GraphicsContext& context) {
	RenderState& renderState = RenderState::get(context);
	renderState.getTexture().push(this);
	//use(context);
}

void Texture::finishRender(const GraphicsContext& context) {
	RenderState& renderState = RenderState::get(context);
	//release(context);
	renderState.getTexture().pop();
}

void Texture::use(const GraphicsContext& context) {
	glBindTexture(getTarget(context), getId(context));
}

void Texture::release(const GraphicsContext& context) {
	glBindTexture(getTarget(context), 0);
}

GLuint Texture::getFormat() const {
	return format;
}

GLuint Texture::getInternalFormat() const {
	return internalFormat;
}

GLuint Texture::getType() const {
	return GL_UNSIGNED_BYTE;
}

GLuint Texture::getAttachmentType() const {
	return GL_COLOR_ATTACHMENT0;
}

}