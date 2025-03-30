#include "BloomEffect.h"

BloomEffect::BloomEffect()
	: shaders_()
	, brightnessTexture_()
	, firstPassTextures_()
	, secondPassTextures_()
{
	shaders_.load(Shaders::BrightnessPass, "Media/Shaders/Fullpass.vert", "Media/Shaders/Brightness.frag");
	shaders_.load(Shaders::DownSamplePass, "Media/Shaders/Fullpass.vert", "Media/Shaders/DownSample.frag");
	shaders_.load(Shaders::GaussianBlurPass, "Media/Shaders/Fullpass.vert", "Media/Shaders/GuassianBlur.frag");
	shaders_.load(Shaders::AddPass, "Media/Shaders/Fullpass.vert", "Media/Shaders/Add.frag");
}

void BloomEffect::apply(const sf::RenderTexture& input, sf::RenderTarget& output)
{
	// Prepare the textures for the bloom effect
	prepareTextures(input.getSize());

	// Filter the bright areas of the scene
	filterBright(input, brightnessTexture_);

	// Downsample the bright areas
	downsample(brightnessTexture_, firstPassTextures_[0]);
	// Apply a Gaussian blur to the downsampled texture
	blurMultipass(firstPassTextures_);

	// Downsample the blurred texture
	downsample(firstPassTextures_[0], secondPassTextures_[0]);
	// Apply a Gaussian blur to the downsampled texture
	blurMultipass(secondPassTextures_);

	add(firstPassTextures_[0], secondPassTextures_[0], firstPassTextures_[1]);
	firstPassTextures_[1].display();

	// Add the blurred bloom effect to the original scene
	add(input, secondPassTextures_[1], output);
}

void BloomEffect::prepareTextures(sf::Vector2u size)
{
	if (brightnessTexture_.getSize() != size)
	{
		brightnessTexture_.create(size.x, size.y);
		brightnessTexture_.setSmooth(true);

		firstPassTextures_[0].create(size.x / 2, size.y / 2);
		firstPassTextures_[0].setSmooth(true);
		firstPassTextures_[1].create(size.x / 2, size.y / 2);
		firstPassTextures_[1].setSmooth(true);

		secondPassTextures_[0].create(size.x / 4, size.y / 4);
		secondPassTextures_[0].setSmooth(true);
		secondPassTextures_[1].create(size.x / 4, size.y / 4);
		secondPassTextures_[1].setSmooth(true);
	}
}

void BloomEffect::filterBright(const sf::RenderTexture& input, sf::RenderTexture& output)
{
	sf::Shader& brightness = shaders_.get(Shaders::BrightnessPass);

	brightness.setUniform("source", input.getTexture());
	applyShader(brightness, output);
	output.display();
}

void BloomEffect::blurMultipass(RenderTextureArray& renderTextures)
{
	sf::Vector2u textureSize = renderTextures[0].getSize();

	for (std::size_t count = 0; count != 2; ++count)
	{
		// Apply the Gaussian blur to the first pass
		blur(renderTextures[0], renderTextures[1], sf::Vector2f(0.f, 1.f / textureSize.y));
		blur(renderTextures[1], renderTextures[0], sf::Vector2f(1.f / textureSize.x, 0.f));
	}
}

void BloomEffect::blur(const sf::RenderTexture& input, sf::RenderTexture& output, sf::Vector2f offsetFactor)
{
	sf::Shader& gaussianBlur = shaders_.get(Shaders::GaussianBlurPass);

	gaussianBlur.setUniform("source", input.getTexture());
	gaussianBlur.setUniform("offsetFactor", offsetFactor);
	applyShader(gaussianBlur, output);
	output.display();
}

void BloomEffect::downsample(const sf::RenderTexture& input, sf::RenderTexture& output)
{
	sf::Shader& downSample = shaders_.get(Shaders::DownSamplePass);

	downSample.setUniform("source", input.getTexture());
	downSample.setUniform("sourceSize", sf::Vector2f(input.getSize()));
	applyShader(downSample, output);
	output.display();
}

void BloomEffect::add(const sf::RenderTexture& source, const sf::RenderTexture& bloom, sf::RenderTarget& output)
{
	sf::Shader& add = shaders_.get(Shaders::AddPass);

	add.setUniform("source", source.getTexture());
	add.setUniform("bloom", bloom.getTexture());
	applyShader(add, output);
}
