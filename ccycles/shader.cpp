/**
Copyright 2014-2015 Robert McNeel and Associates

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
**/

#include "internal_types.h"

extern std::vector<CCScene> scenes;

std::vector<CCShader*> shaders;

std::vector<CCImage*> images;

void _init_shaders()
{
	cycles_create_shader(0); // default surface
	cycles_create_shader(0); // default light
	cycles_create_shader(0); // default background
	cycles_create_shader(0); // default empty
}

void _cleanup_shaders()
{
	for (CCShader* sh : shaders) {
		// just setting to nullptr, as scene disposal frees this memory.
		sh->graph = nullptr;
		sh->shader = nullptr;
		sh->scene_mapping.clear();
		delete sh;
	}
	shaders.clear();
}

void _cleanup_images()
{
	for (CCImage* img : images) {
		if (img == nullptr) continue;

		delete [] img->builtin_data;
		img->builtin_data = nullptr;

		delete img;
	}
	images.clear();
}

/* Create a new shader.
 TODO: name for shader
*/
unsigned int cycles_create_shader(unsigned int client_id)
{
	CCShader* sh = new CCShader();
	sh->shader->graph = sh->graph;
	shaders.push_back(sh);

	return (unsigned int)(shaders.size() - 1);
}

/* Add shader to specified scene. */
unsigned int cycles_scene_add_shader(unsigned int client_id, unsigned int scene_id, unsigned int shader_id)
{
	SCENE_FIND(scene_id)
		CCShader* sh = shaders[shader_id];
		sce->shaders.push_back(sh->shader);
		sh->shader->tag_update(sce);
		unsigned int shid = (unsigned int)(sce->shaders.size() - 1);
		sh->scene_mapping.insert({ scene_id, shid });
		return shid;
	SCENE_FIND_END()

	return (unsigned int)(-1);
}

void cycles_scene_tag_shader(unsigned int client_id, unsigned int scene_id, unsigned int shader_id)
{
	SCENE_FIND(scene_id)
		CCShader* sh = shaders[shader_id];
		sh->shader->tag_update(sce);
	SCENE_FIND_END()
}

/* Get Cycles shader ID in specific scene. */
unsigned int cycles_scene_shader_id(unsigned int client_id, unsigned int scene_id, unsigned int shader_id)
{
	CCShader* sh = shaders[shader_id];
	if (sh->scene_mapping.find(scene_id) != sh->scene_mapping.end()) {
		return sh->scene_mapping[scene_id];
	}

	return (unsigned int)(-1);
}

void cycles_shader_new_graph(unsigned int client_id, unsigned int shader_id)
{
		CCShader* sh = shaders[shader_id];
		sh->graph = new ccl::ShaderGraph();
		sh->shader->set_graph(sh->graph);
}


void cycles_shader_set_name(unsigned int client_id, unsigned int shader_id, const char* name)
{
	SHADER_SET(shader_id, string, name, name);
}

void cycles_shader_set_use_mis(unsigned int client_id, unsigned int shader_id, unsigned int use_mis)
{
	SHADER_SET(shader_id, bool, use_mis, use_mis == 1)
}

void cycles_shader_set_use_transparent_shadow(unsigned int client_id, unsigned int shader_id, unsigned int use_transparent_shadow)
{
	SHADER_SET(shader_id, bool, use_transparent_shadow, use_transparent_shadow == 1)
}

void cycles_shader_set_heterogeneous_volume(unsigned int client_id, unsigned int shader_id, unsigned int heterogeneous_volume)
{
	SHADER_SET(shader_id, bool, heterogeneous_volume, heterogeneous_volume == 1)
}

unsigned int cycles_add_shader_node(unsigned int client_id, unsigned int shader_id, shadernode_type shn_type)
{
	ccl::ShaderNode* node = nullptr;
	switch (shn_type) {
		case shadernode_type::BACKGROUND:
			node = new ccl::BackgroundNode();
			break;
		case shadernode_type::DIFFUSE:
			node = new ccl::DiffuseBsdfNode();
			break;
		case shadernode_type::ANISOTROPIC:
			node = new ccl::AnisotropicBsdfNode();
			break;
		case shadernode_type::TRANSLUCENT:
			node = new ccl::TranslucentBsdfNode();
			break;
		case shadernode_type::TRANSPARENT:
			node = new ccl::TransparentBsdfNode();
			break;
		case shadernode_type::VELVET:
			node = new ccl::VelvetBsdfNode();
			break;
		case shadernode_type::TOON:
			node = new ccl::ToonBsdfNode();
			break;
		case shadernode_type::GLOSSY:
			node = new ccl::GlossyBsdfNode();
			break;
		case shadernode_type::GLASS:
			node = new ccl::GlassBsdfNode();
			break;
		case shadernode_type::REFRACTION:
		{
			ccl::RefractionBsdfNode* refrnode = new ccl::RefractionBsdfNode();
			refrnode->distribution = OpenImageIO::v1_3::ustring("Beckmann");
			node = dynamic_cast<ccl::ShaderNode *>(refrnode);
		}
			break;
		case shadernode_type::HAIR:
			node = new ccl::HairBsdfNode();
			break;
		case shadernode_type::EMISSION:
			node = new ccl::EmissionNode();
			break;
		case shadernode_type::AMBIENT_OCCLUSION:
			node = new ccl::AmbientOcclusionNode();
			break;
		case shadernode_type::ABSORPTION_VOLUME:
			node = new ccl::AbsorptionVolumeNode();
			break;
		case shadernode_type::SCATTER_VOLUME:
			node = new ccl::ScatterVolumeNode();
			break;
		case shadernode_type::SUBSURFACE_SCATTERING:
			node = new ccl::SubsurfaceScatteringNode();
			break;
		case shadernode_type::VALUE:
			node = new ccl::ValueNode();
			break;
		case shadernode_type::COLOR:
			node = new ccl::ColorNode();
			break;
		case shadernode_type::MIX_CLOSURE:
			node = new ccl::MixClosureNode();
			break;
		case shadernode_type::ADD_CLOSURE:
			node = new ccl::AddClosureNode();
			break;
		case shadernode_type::INVERT:
			node = new ccl::InvertNode();
			break;
		case shadernode_type::MIX:
			node = new ccl::MixNode();
			break;
		case shadernode_type::GAMMA:
			node = new ccl::GammaNode();
			break;
		case shadernode_type::WAVELENGTH:
			node = new ccl::WavelengthNode();
			break;
		case shadernode_type::BLACKBODY:
			node = new ccl::BlackbodyNode();
			break;
		case shadernode_type::CAMERA:
			node = new ccl::CameraNode();
			break;
		case shadernode_type::FRESNEL:
			node = new ccl::FresnelNode();
			break;
		case shadernode_type::MATH:
			node = new ccl::MathNode();
			break;
		case shadernode_type::IMAGE_TEXTURE:
			node = new ccl::ImageTextureNode();
			break;
		case shadernode_type::ENVIRONMENT_TEXTURE:
			node = new ccl::EnvironmentTextureNode();
			break;
		case shadernode_type::BRICK_TEXTURE:
			node = new ccl::BrickTextureNode();
			break;
		case shadernode_type::SKY_TEXTURE:
			node = new ccl::SkyTextureNode();
			break;
		case shadernode_type::CHECKER_TEXTURE:
			node = new ccl::CheckerTextureNode();
			break;
		case shadernode_type::NOISE_TEXTURE:
			node = new ccl::NoiseTextureNode();
			break;
		case shadernode_type::WAVE_TEXTURE:
			node = new ccl::WaveTextureNode();
			break;
		case shadernode_type::MAGIC_TEXTURE:
			node = new ccl::MagicTextureNode();
			break;
		case shadernode_type::MUSGRAVE_TEXTURE:
			node = new ccl::MusgraveTextureNode();
			break;
		case shadernode_type::TEXTURE_COORDINATE:
			node = new ccl::TextureCoordinateNode();
			break;
		case shadernode_type::BUMP:
			node = new ccl::BumpNode();
			break;
		case shadernode_type::RGBTOBW:
			node = new ccl::ConvertNode(ccl::SHADER_SOCKET_COLOR, ccl::SHADER_SOCKET_FLOAT);
			break;
		case shadernode_type::RGBTOLUMINANCE:
			node = new ccl::ConvertNode(ccl::SHADER_SOCKET_COLOR2, ccl::SHADER_SOCKET_FLOAT);
			break;
		case shadernode_type::LIGHTPATH:
			node = new ccl::LightPathNode();
			break;
		case shadernode_type::LIGHTFALLOFF:
			node = new ccl::LightFalloffNode();
			break;
		case shadernode_type::VORONOI_TEXTURE:
			node = new ccl::VoronoiTextureNode();
			break;
		case shadernode_type::LAYERWEIGHT:
			node = new ccl::LayerWeightNode();
			break;
		case shadernode_type::COMBINE_XYZ:
			node = new ccl::CombineXYZNode();
			break;
		case shadernode_type::SEPARATE_XYZ:
			node = new ccl::SeparateXYZNode();
			break;
		case shadernode_type::HSV_SEPARATE:
			node = new ccl::SeparateHSVNode();
			break;
		case shadernode_type::HSV_COMBINE:
			node = new ccl::CombineHSVNode();
			break;
		case shadernode_type::RGB_SEPARATE:
			node = new ccl::SeparateRGBNode();
			break;
		case shadernode_type::RGB_COMBINE:
			node = new ccl::CombineRGBNode();
			break;
		case shadernode_type::MAPPING:
			node = new ccl::MappingNode();
			break;
		case shadernode_type::HOLDOUT:
			node = new ccl::HoldoutNode();
			break;
		case shadernode_type::HUE_SAT:
			node = new ccl::HSVNode();
			break;
		case shadernode_type::GRADIENT_TEXTURE:
			node = new ccl::GradientTextureNode();
			break;
		case shadernode_type::COLOR_RAMP:
			node = new ccl::RGBRampNode();
			break;
		case shadernode_type::VECT_MATH:
			node = new ccl::VectorMathNode();
			break;
		case shadernode_type::MATRIX_MATH:
			node = new ccl::MatrixMathNode();
			break;
	}

	if (node) {
		shaders[shader_id]->graph->add(node);
		return (unsigned int)(node->id);
	}
	else {
		return (unsigned int)-1;
	}
}

enum class attr_type {
	INT,
	FLOAT,
	FLOAT4,
	CHARP
};

struct attrunion {
	attr_type type;
	union {
		int i;
		float f;
		ccl::float4 f4;
		const char* cp;
	};
};

void shadernode_set_attribute(unsigned int client_id, unsigned int shader_id, unsigned int shnode_id, const char* attribute_name, attrunion v)
{
	auto attr = string(attribute_name);
	SHADERNODE_FIND(shader_id, shnode_id)
			for (ccl::ShaderInput* inp : (*psh)->inputs) {
				auto inpname = string(inp->name);
				if (ccl::string_iequals(inpname, attribute_name)) {
					switch (v.type) {
					case attr_type::INT:
						inp->value.x = (float)v.i;
						logger.logit(client_id, "shader_id: ", shader_id, " -> shnode_id: ", shnode_id, " |> setting attribute: ", attribute_name, " to: ", v.i);
						break;
					case attr_type::FLOAT:
						inp->value.x = v.f;
						logger.logit(client_id, "shader_id: ", shader_id, " -> shnode_id: ", shnode_id, " |> setting attribute: ", attribute_name, " to: ", v.f);
						break;
					case attr_type::FLOAT4:
						inp->value.x = v.f4.x;
						inp->value.y = v.f4.y;
						inp->value.z = v.f4.z;
						logger.logit(client_id, "shader_id: ", shader_id, " -> shnode_id: ", shnode_id, " |> setting attribute: ", attribute_name, " to: ", v.f4.x, ",", v.f4.y, ",", v.f4.z);
						break;
					case attr_type::CHARP:
						inp->value_string = string(v.cp);
						logger.logit(client_id, "shader_id: ", shader_id, " -> shnode_id: ", shnode_id, " |> setting attribute: ", attribute_name, " to: ", v.cp);
						break;
					}
					return;
				}
			}
			break;
	SHADERNODE_FIND_END()
}

void _set_enum_val(unsigned int client_id, OpenImageIO::v1_3::ustring* str, ccl::ShaderEnum& enm, const OpenImageIO::v1_3::ustring val)
{
	if (enm.exists(val)) {
		*str = val;
	}
	else {
		logger.logit(client_id, "Unknown value ", val);
	}
}

void _set_texture_mapping_transformation(ccl::TextureMapping& mapping, int transform_type, float x, float y, float z)
{
	switch (transform_type) {
		case 0:
			mapping.translation.x = x;
			mapping.translation.y = y;
			mapping.translation.z = z;
			break;
		case 1:
			mapping.rotation.x = x;
			mapping.rotation.y = y;
			mapping.rotation.z = z;
			break;
		case 2:
			mapping.scale.x = x;
			mapping.scale.y = y;
			mapping.scale.z = z;
			break;
	}
}

void cycles_shadernode_texmapping_set_transformation(unsigned int client_id, unsigned int shader_id, unsigned int shnode_id, shadernode_type shn_type, int transform_type, float x, float y, float z)
{
	SHADERNODE_FIND(shader_id, shnode_id)
		string tp{ "UNKNOWN" };
		switch (transform_type) {
			case 0:
				tp = "TRANSLATION";
				break;
			case 1:
				tp = "ROTATION";
				break;
			case 2:
				tp = "SCALE";
				break;
		}
		logger.logit(client_id, "Setting texture map transformation (", tp, ") to ", x, ",", y, ",", z, " for shadernode type ", shn_type);
			switch (shn_type) {
				case shadernode_type::MAPPING:
					ccl::MappingNode* node = dynamic_cast<ccl::MappingNode*>(*psh);
					_set_texture_mapping_transformation(node->tex_mapping, transform_type, x, y, z);
					break;
			}
	SHADERNODE_FIND_END()
}

void cycles_shadernode_texmapping_set_mapping(unsigned int client_id, unsigned int shader_id, unsigned int shnode_id, shadernode_type shn_type, ccl::TextureMapping::Mapping x, ccl::TextureMapping::Mapping y, ccl::TextureMapping::Mapping z)
{
	SHADERNODE_FIND(shader_id, shnode_id)
		logger.logit(client_id, "Setting texture map mapping to ", x, ",", y, ",", z, " for shadernode type ", shn_type);
			switch (shn_type) {
				case shadernode_type::MAPPING:
					ccl::MappingNode* node = dynamic_cast<ccl::MappingNode*>(*psh);
					node->tex_mapping.x_mapping = x;
					node->tex_mapping.y_mapping = y;
					node->tex_mapping.z_mapping = z;
					break;
			}
	SHADERNODE_FIND_END()
}

void cycles_shadernode_texmapping_set_projection(unsigned int client_id, unsigned int shader_id, unsigned int shnode_id, shadernode_type shn_type, ccl::TextureMapping::Projection tm_projection)
{
	SHADERNODE_FIND(shader_id, shnode_id)
		logger.logit(client_id, "Setting texture map projection type to ", tm_projection, " for shadernode type ", shn_type);
			switch (shn_type) {
				case shadernode_type::MAPPING:
					ccl::MappingNode* node = dynamic_cast<ccl::MappingNode*>(*psh);
					node->tex_mapping.projection= tm_projection;
					break;
			}
	SHADERNODE_FIND_END()
}

void cycles_shadernode_texmapping_set_type(unsigned int client_id, unsigned int shader_id, unsigned int shnode_id, shadernode_type shn_type, ccl::TextureMapping::Type tm_type)
{
	SHADERNODE_FIND(shader_id, shnode_id)
		logger.logit(client_id, "Setting texture map type to ", tm_type, " for shadernode type ", shn_type);
			switch (shn_type) {
				case shadernode_type::MAPPING:
					ccl::MappingNode* node = dynamic_cast<ccl::MappingNode*>(*psh);
					node->tex_mapping.type = tm_type;
					break;
			}
	SHADERNODE_FIND_END()
}

/* TODO: add all enum possibilities.
 */
void cycles_shadernode_set_enum(unsigned int client_id, unsigned int shader_id, unsigned int shnode_id, shadernode_type shn_type, const char* enum_name, const char* value)
{
	auto val = OpenImageIO::v1_3::ustring(value);
	auto ename = string{enum_name};

	SHADERNODE_FIND(shader_id, shnode_id)
			switch (shn_type) {
				case shadernode_type::MATH:
					{
						ccl::MathNode* node = dynamic_cast<ccl::MathNode*>(*psh);
						_set_enum_val(client_id, &node->type, ccl::MathNode::type_enum, val);
					}
					break;
				case shadernode_type::VECT_MATH:
					{
						ccl::VectorMathNode *node = dynamic_cast<ccl::VectorMathNode*>(*psh);
						_set_enum_val(client_id, &node->type, ccl::VectorMathNode::type_enum, val);
					}
					break;
				case shadernode_type::MATRIX_MATH:
					{
						ccl::MatrixMathNode *node = dynamic_cast<ccl::MatrixMathNode*>(*psh);
						_set_enum_val(client_id, &node->type, ccl::MatrixMathNode::type_enum, val);
					}
					break;
				case shadernode_type::MIX:
					{
						ccl::MixNode* node = dynamic_cast<ccl::MixNode*>(*psh);
						_set_enum_val(client_id, &node->type, ccl::MixNode::type_enum, val);
					}
					break;
				case shadernode_type::REFRACTION:
					{
						ccl::RefractionBsdfNode* node = dynamic_cast<ccl::RefractionBsdfNode*>(*psh);
						_set_enum_val(client_id, &node->distribution, ccl::RefractionBsdfNode::distribution_enum, val);
					}
					break;
				case shadernode_type::GLOSSY:
					{
						ccl::GlossyBsdfNode* node = dynamic_cast<ccl::GlossyBsdfNode*>(*psh);
						_set_enum_val(client_id, &node->distribution, ccl::GlossyBsdfNode::distribution_enum, val);
					}
					break;
				case shadernode_type::GLASS:
					{
						ccl::GlassBsdfNode* node = dynamic_cast<ccl::GlassBsdfNode*>(*psh);
						_set_enum_val(client_id, &node->distribution, ccl::GlassBsdfNode::distribution_enum, val);
					}
					break;
				case shadernode_type::ANISOTROPIC:
					{
						ccl::AnisotropicBsdfNode* node = dynamic_cast<ccl::AnisotropicBsdfNode*>(*psh);
						_set_enum_val(client_id, &node->distribution, ccl::AnisotropicBsdfNode::distribution_enum, val);
					}
					break;
				case shadernode_type::WAVE_TEXTURE:
					{
						ccl::WaveTextureNode* node = dynamic_cast<ccl::WaveTextureNode*>(*psh);
						_set_enum_val(client_id, &node->type, ccl::WaveTextureNode::type_enum, val);
					}
					break;
				case shadernode_type::VORONOI_TEXTURE:
					{
						ccl::VoronoiTextureNode* node = dynamic_cast<ccl::VoronoiTextureNode*>(*psh);
						_set_enum_val(client_id, &node->coloring, ccl::VoronoiTextureNode::coloring_enum, val);
					}
					break;
				case shadernode_type::SKY_TEXTURE:
					{
						ccl::SkyTextureNode* node = dynamic_cast<ccl::SkyTextureNode*>(*psh);
						_set_enum_val(client_id, &node->type, ccl::SkyTextureNode::type_enum, val);
					}
					break;
				case shadernode_type::ENVIRONMENT_TEXTURE:
					{
						ccl::EnvironmentTextureNode* node = dynamic_cast<ccl::EnvironmentTextureNode*>(*psh);
						if (ename=="color_space") {
							_set_enum_val(client_id, &node->color_space, ccl::EnvironmentTextureNode::color_space_enum, val);
						}
						else if (ename=="projection") {
							_set_enum_val(client_id, &node->projection, ccl::EnvironmentTextureNode::projection_enum, val);
						}
					}
					break;
				case shadernode_type::IMAGE_TEXTURE:
					{
						ccl::ImageTextureNode* node = dynamic_cast<ccl::ImageTextureNode*>(*psh);
						if (ename=="color_space") {
							_set_enum_val(client_id, &node->color_space, ccl::ImageTextureNode::color_space_enum, val);
						}
						else if (ename=="projection") {
							_set_enum_val(client_id, &node->projection, ccl::ImageTextureNode::projection_enum, val);
						}
						break;
					}
				case shadernode_type::GRADIENT_TEXTURE:
					{
						ccl::GradientTextureNode* node = dynamic_cast<ccl::GradientTextureNode*>(*psh);
						_set_enum_val(client_id, &node->type, ccl::GradientTextureNode::type_enum, val);
						break;
					}
			}
			break;
	SHADERNODE_FIND_END()
}

CCImage* find_existing_ccimage(string imgname, unsigned int width, unsigned int height, unsigned int depth, unsigned int channels, bool is_float)
{
	CCImage* existing_image = nullptr;
	for (CCImage* im : images) {
		if (im->filename == imgname
			&& im->width == (int)width
			&& im->height== (int)height
			&& im->depth == (int)depth
			&& im->channels == (int)channels
			&& im->is_float == is_float
			) {
			existing_image = im;
			break;
		}
	}
	return existing_image;
}

template <class T>
CCImage* get_ccimage(string imgname, T* img, unsigned int width, unsigned int height, unsigned int depth, unsigned int channels, bool is_float)
{
	CCImage* existing_image = find_existing_ccimage(imgname, width, height, depth, channels, is_float);
	CCImage* nimg = existing_image ? existing_image : new CCImage();
	if (!existing_image) {
		T* imgdata = new T[width*height*channels*depth];
		memcpy(imgdata, img, sizeof(T)*width*height*channels*depth);
		nimg->builtin_data = imgdata;
		nimg->filename = imgname;
		nimg->width = (int)width;
		nimg->height = (int)height;
		nimg->depth = (int)depth;
		nimg->channels = (int)channels;
		nimg->is_float = is_float;
		images.push_back(nimg);
	}
	else {
		memcpy(existing_image->builtin_data, img, sizeof(T)*width*height*channels*depth);
	}


	return nimg;
}

void cycles_shadernode_set_member_float_img(unsigned int client_id, unsigned int shader_id, unsigned int shnode_id, shadernode_type shn_type, const char* member_name, const char* img_name, float* img, unsigned int width, unsigned int height, unsigned int depth, unsigned int channels)
{
	auto mname = string{ member_name };
	auto imname = string{ img_name };

	SHADERNODE_FIND(shader_id, shnode_id)
			switch (shn_type) {
				case shadernode_type::IMAGE_TEXTURE:
					{
						CCImage* nimg = get_ccimage<float>(imname, img, width, height, depth, channels, true);
						ccl::ImageTextureNode* imtex = dynamic_cast<ccl::ImageTextureNode*>(*psh);
						imtex->builtin_data = nimg;
						imtex->interpolation = ccl::InterpolationType::INTERPOLATION_LINEAR;
						imtex->filename = nimg->filename;
					}	
					break;
				case shadernode_type::ENVIRONMENT_TEXTURE:
					{
						CCImage* nimg = get_ccimage<float>(imname, img, width, height, depth, channels, true);
						ccl::EnvironmentTextureNode* envtex = dynamic_cast<ccl::EnvironmentTextureNode*>(*psh);
						envtex->builtin_data = nimg;
						envtex->filename = nimg->filename;
					}	
					break;
			}
	SHADERNODE_FIND_END()
}

void cycles_shadernode_set_member_byte_img(unsigned int client_id, unsigned int shader_id, unsigned int shnode_id, shadernode_type shn_type, const char* member_name, const char* img_name, unsigned char* img, unsigned int width, unsigned int height, unsigned int depth, unsigned int channels)
{
	auto mname = string{ member_name };
	auto imname = string{ img_name };
	SHADERNODE_FIND(shader_id, shnode_id)
			switch (shn_type) {
				case shadernode_type::IMAGE_TEXTURE:
					{
						CCImage* nimg = get_ccimage<unsigned char>(imname, img, width, height, depth, channels, false);
						ccl::ImageTextureNode* imtex = dynamic_cast<ccl::ImageTextureNode*>(*psh);
						imtex->builtin_data = nimg;
						imtex->filename = nimg->filename;
					}
					break;
				case shadernode_type::ENVIRONMENT_TEXTURE:
					{
						CCImage* nimg = get_ccimage<unsigned char>(imname, img, width, height, depth, channels, false);
						ccl::EnvironmentTextureNode* envtex = dynamic_cast<ccl::EnvironmentTextureNode*>(*psh);
						envtex->builtin_data = nimg;
						envtex->filename = nimg->filename;
					}
					break;
			}
	SHADERNODE_FIND_END()
}

void cycles_shadernode_set_member_bool(unsigned int client_id, unsigned int shader_id, unsigned int shnode_id, shadernode_type shn_type, const char* member_name, bool value)
{
	auto mname = string{ member_name };

	SHADERNODE_FIND(shader_id, shnode_id)
			switch (shn_type) {
				case shadernode_type::MATH:
					{
						ccl::MathNode* mnode = dynamic_cast<ccl::MathNode*>(*psh);
						mnode->use_clamp = value;
					}
					break;
				case shadernode_type::MAPPING:
					{
						ccl::MappingNode* mapping = dynamic_cast<ccl::MappingNode*>(*psh);
						if (mname == "useminmax") {
							mapping->tex_mapping.use_minmax = value;
						}
					}
					break;
				case shadernode_type::COLOR_RAMP:
					{
						ccl::RGBRampNode* colorramp = dynamic_cast<ccl::RGBRampNode*>(*psh);
						if (mname == "interpolate")
						{
							colorramp->interpolate = value;
						}
					}
					break;
				case shadernode_type::BUMP:
					{
						ccl::BumpNode* bump = dynamic_cast<ccl::BumpNode*>(*psh);
						if (mname == "invert") {
							bump->invert = value;
						}
					}
					break;
				case shadernode_type::IMAGE_TEXTURE:
					{
						ccl::ImageTextureNode* imgtex= dynamic_cast<ccl::ImageTextureNode*>(*psh);
						if (mname == "use_alpha") {
							imgtex->use_alpha = value;
						}
						else if (mname == "is_linear") {
							imgtex->is_linear = value;
						}
					}
					break;
				case shadernode_type::ENVIRONMENT_TEXTURE:
					{
						ccl::EnvironmentTextureNode* envtex= dynamic_cast<ccl::EnvironmentTextureNode*>(*psh);
						if (mname == "is_linear") {
							envtex->is_linear = value;
						}
					}
					break;
				case shadernode_type::TEXTURE_COORDINATE:
					{
						ccl::TextureCoordinateNode* texco = dynamic_cast<ccl::TextureCoordinateNode*>(*psh);
						if (mname == "use_transform") {
							texco->use_transform = value;
						}
					}
					break;
				case shadernode_type::MIX:
					{
						ccl::MixNode* mix = dynamic_cast<ccl::MixNode*>(*psh);
						if (mname == "use_clamp") {
							mix->use_clamp = value;
						}
					}
					break;
			}
	SHADERNODE_FIND_END()
}

void cycles_shadernode_set_member_int(unsigned int client_id, unsigned int shader_id, unsigned int shnode_id, shadernode_type shn_type, const char* member_name, int value)
{
	auto mname = string{ member_name };
	SHADERNODE_FIND(shader_id, shnode_id)
			switch (shn_type) {
				case shadernode_type::BRICK_TEXTURE:
					{
						ccl::BrickTextureNode* bricknode = dynamic_cast<ccl::BrickTextureNode*>(*psh);
						if (mname == "offset_frequency")
							bricknode->offset_frequency = value;
						else if (mname == "squash_frequency")
							bricknode->squash_frequency = value;
					}
					break;
				case shadernode_type::IMAGE_TEXTURE:
					{
						ccl::ImageTextureNode* imgnode = dynamic_cast<ccl::ImageTextureNode*>(*psh);
						if (mname == "interpolation") {
							imgnode->interpolation = (ccl::InterpolationType)value;
						}
					}
					break;
				case shadernode_type::ENVIRONMENT_TEXTURE:
					{
						ccl::EnvironmentTextureNode* envnode = dynamic_cast<ccl::EnvironmentTextureNode*>(*psh);
						if (mname == "interpolation") {
							envnode->interpolation = (ccl::InterpolationType)value;
						}
					}
					break;
				case shadernode_type::MAGIC_TEXTURE:
					{
						ccl::MagicTextureNode* envnode = dynamic_cast<ccl::MagicTextureNode*>(*psh);
						if (mname == "depth") {
							envnode->depth = value;
						}
					}
					break;
			}
	SHADERNODE_FIND_END()
}


void cycles_shadernode_set_member_float(unsigned int client_id, unsigned int shader_id, unsigned int shnode_id, shadernode_type shn_type, const char* member_name, float value)
{
	auto mname = string{ member_name };

	SHADERNODE_FIND(shader_id, shnode_id)
			switch (shn_type) {
				case shadernode_type::VALUE:
					{
						ccl::ValueNode* valuenode = dynamic_cast<ccl::ValueNode*>(*psh);
						valuenode->value = value;
					}
					break;
				case shadernode_type::IMAGE_TEXTURE:
					{
						ccl::ImageTextureNode* imtexnode = dynamic_cast<ccl::ImageTextureNode*>(*psh);
						if (mname == "projection_blend") {
							imtexnode->projection_blend = value;
						}
					}
					break;
				case shadernode_type::BRICK_TEXTURE:
					{
						ccl::BrickTextureNode* bricknode = dynamic_cast<ccl::BrickTextureNode*>(*psh);
						if (mname == "offset")
							bricknode->offset = value;
						else if (mname == "squash")
							bricknode->squash = value;
					}
					break;
				case shadernode_type::SKY_TEXTURE:
					{
						ccl::SkyTextureNode* skynode = dynamic_cast<ccl::SkyTextureNode*>(*psh);
						if (mname == "turbidity")
							skynode->turbidity = value;
						else if (mname == "ground_albedo")
							skynode->ground_albedo = value;
					}
					break;
			}
	SHADERNODE_FIND_END()
}

void cycles_shadernode_set_member_vec4_at_index(unsigned int client_id, unsigned int shader_id, unsigned int shnode_id, shadernode_type shn_type, const char* member_name, float x, float y, float z, float w, int index)
{
	auto mname = string{ member_name };

	SHADERNODE_FIND(shader_id, shnode_id)
		switch (shn_type) {
			case shadernode_type::COLOR_RAMP:
				{
					ccl::RGBRampNode* colorramp = dynamic_cast<ccl::RGBRampNode*>(*psh);
					colorramp->ramp[index].x = x;
					colorramp->ramp[index].y = y;
					colorramp->ramp[index].z = z;
					colorramp->ramp[index].w = w;
				}
				break;
			case shadernode_type::TEXTURE_COORDINATE:
				{
					ccl::TextureCoordinateNode* texco = dynamic_cast<ccl::TextureCoordinateNode*>(*psh);
					if (index == 0) {
						texco->ob_tfm.x.x = x;
						texco->ob_tfm.x.y = y;
						texco->ob_tfm.x.z = z;
						texco->ob_tfm.x.w = w;
					}
					if (index == 1) {
						texco->ob_tfm.y.x = x;
						texco->ob_tfm.y.y = y;
						texco->ob_tfm.y.z = z;
						texco->ob_tfm.y.w = w;
					}
					if (index == 2) {
						texco->ob_tfm.z.x = x;
						texco->ob_tfm.z.y = y;
						texco->ob_tfm.z.z = z;
						texco->ob_tfm.z.w = w;
					}
					if (index == 3) {
						texco->ob_tfm.w.x = x;
						texco->ob_tfm.w.y = y;
						texco->ob_tfm.w.z = z;
						texco->ob_tfm.w.w = w;
					}
				}
				break;
			case shadernode_type::MATRIX_MATH:
				{
					ccl::MatrixMathNode* matmath = dynamic_cast<ccl::MatrixMathNode*>(*psh);
					if (index == 0) {
						matmath->tfm.x.x = x;
						matmath->tfm.x.y = y;
						matmath->tfm.x.z = z;
						matmath->tfm.x.w = w;
					}
					if (index == 1) {
						matmath->tfm.y.x = x;
						matmath->tfm.y.y = y;
						matmath->tfm.y.z = z;
						matmath->tfm.y.w = w;
					}
					if (index == 2) {
						matmath->tfm.z.x = x;
						matmath->tfm.z.y = y;
						matmath->tfm.z.z = z;
						matmath->tfm.z.w = w;
					}
					if (index == 3) {
						matmath->tfm.w.x = x;
						matmath->tfm.w.y = y;
						matmath->tfm.w.z = z;
						matmath->tfm.w.w = w;
					}
				}
				break;
		}
	SHADERNODE_FIND_END()
}

void cycles_shadernode_set_member_vec(unsigned int client_id, unsigned int shader_id, unsigned int shnode_id, shadernode_type shn_type, const char* member_name, float x, float y, float z)
{
	auto mname = string{ member_name };

	SHADERNODE_FIND(shader_id, shnode_id)
			switch (shn_type) {
			case shadernode_type::COLOR:
					{
						ccl::ColorNode* colnode = dynamic_cast<ccl::ColorNode*>(*psh);
						colnode->value.x = x;
						colnode->value.y = y;
						colnode->value.z = z;
					}
					break;
			case shadernode_type::SKY_TEXTURE:
					{
						ccl::SkyTextureNode* sunnode = dynamic_cast<ccl::SkyTextureNode*>(*psh);
						sunnode->sun_direction.x = x;
						sunnode->sun_direction.y = y;
						sunnode->sun_direction.z = z;
					}
					break;
			case shadernode_type::MAPPING:
				{
					ccl::MappingNode* mapping = dynamic_cast<ccl::MappingNode*>(*psh);
					if (mname == "min") {
						mapping->tex_mapping.min.x = x;
						mapping->tex_mapping.min.y = y;
						mapping->tex_mapping.min.z = z;
					}
					else if (mname == "max") {
						mapping->tex_mapping.max.x = x;
						mapping->tex_mapping.max.y = y;
						mapping->tex_mapping.max.z = z;
					}
				}
				break;
			}
	SHADERNODE_FIND_END()
}

/*
Set an integer attribute with given name to value. shader_id is the global shader ID.
*/
void cycles_shadernode_set_attribute_int(unsigned int client_id, unsigned int shader_id, unsigned int shnode_id, const char* attribute_name, int value)
{
	attrunion v{ attr_type::INT };
	v.i = value;
	shadernode_set_attribute(client_id, shader_id, shnode_id, attribute_name, v);
}

/*
Set a float attribute with given name to value. shader_id is the global shader ID.
*/
void cycles_shadernode_set_attribute_float(unsigned int client_id, unsigned int shader_id, unsigned int shnode_id, const char* attribute_name, float value)
{
	attrunion v{ attr_type::FLOAT };
	v.f = value;
	shadernode_set_attribute(client_id, shader_id, shnode_id, attribute_name, v);
}

/*
Set a vector of floats attribute with given name to x, y and z. shader_id is the global shader ID.
*/
void cycles_shadernode_set_attribute_vec(unsigned int client_id, unsigned int shader_id, unsigned int shnode_id, const char* attribute_name, float x, float y, float z)
{
	attrunion v{ attr_type::FLOAT4 };
	v.f4.x = x;
	v.f4.y = y;
	v.f4.z = z;
	shadernode_set_attribute(client_id, shader_id, shnode_id, attribute_name, v);
}

/*
Set a string attribute with given name to value. shader_id is the global shader ID.
*/
void cycles_shadernode_set_attribute_string(unsigned int client_id, unsigned int shader_id, unsigned int shnode_id, const char* attribute_name, const char* value)
{
	attrunion v;
	v.type = attr_type::CHARP;
	v.cp = value;
	shadernode_set_attribute(client_id, shader_id, shnode_id, attribute_name, v);
}

void cycles_shader_connect_nodes(unsigned int client_id, unsigned int shader_id, unsigned int from_id, const char* from, unsigned int to_id, const char* to)
{
	CCShader* sh = shaders[shader_id];
	auto shfrom = sh->graph->nodes.begin();
	auto shfrom_end = sh->graph->nodes.end();
	auto shto = sh->graph->nodes.begin();
	auto shto_end = sh->graph->nodes.end();

	while (shfrom != shfrom_end) {
		if ((*shfrom)->id == from_id) {
			break;
		}
		++shfrom;
	}

	while (shto != shto_end) {
		if ((*shto)->id == to_id) {
			break;
		}
		++shto;
	}

	if (shfrom == shfrom_end || shto == shto_end) {
		return; // TODO: figure out what to do on errors like this
	}
	logger.logit(client_id, "Shader ", shader_id, " :: ", from_id, ":", from, " -> ", to_id, ":", to);

	sh->graph->connect((*shfrom)->output(from), (*shto)->input(to));
}

