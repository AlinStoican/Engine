#pragma once

#include <Core.h>
#include <Texture.h>
#include <Shader.h>
#include <Camera.h>
#include <GraphicModel.h>
#include <algorithm>
#include <ErrorReporting.h>

//todo get mesh material?

namespace gl3d
{
	namespace internal
	{

		//todo probably just keep a counter and get the next one
		template <class T>
		int generateNewIndex(T indexesVec)
		{
			int id = 0;

			auto indexesCopy = indexesVec;
			std::sort(indexesCopy.begin(), indexesCopy.end());

			if (indexesCopy.empty())
			{
				id = 1;
			}
			else
			{
				id = 1;

				for (int i = 0; i < indexesCopy.size(); i++)
				{
					if (indexesCopy[i] != id)
					{
						break;
					}
					else
					{
						id++;
					}
				}

			}
			
			return id;
		};

	};

	struct AtmosfericScatteringSettings
	{
		glm::vec3 sun = {0,1,0};
		//sky
		glm::vec3 color1 = {0,0,0};
		//sun
		glm::vec3 color2 = {0,0,0};
		glm::vec3 ground = {0,0,0};
		float g = 0;
		bool useGroundColor = 0;
	};

	//load using renderer.loadColorLookupTextureFromFile
	//the memory of the lookup texture is not managed by the engine and not cleared by the engine on clearAll.
	struct ColorLookupTexture
	{
		GpuTexture t;
		void clear() { t.clear(); }
	};

	struct Renderer3D

	{
		//todo store the default fbo
		//size of the screen and the default frameBuffer
		void init(int x, int y, const char *BRDFIntegrationMapFileLocation, GLuint frameBuffer = 0);
		
		GLuint frameBuffer = 0;

		ErrorReporter errorReporter;
		FileOpener fileOpener;

		ErrorCallback_t *setErrorCallback(ErrorCallback_t *errorCallback, void *userData);
		ErrorCallback_t *getErrorCallback();

	#pragma region material
		
		Material createMaterial(int quality, glm::vec4 kd = glm::vec4(1),
			float roughness = 0.5f, float metallic = 0.1, float ao = 1.f, float emissive = 0.f, std::string name = "",
			gl3d::Texture albedoTexture = {}, gl3d::Texture normalTexture = {}, gl3d::Texture roughnessTexture = {}, gl3d::Texture metallicTexture = {},
			gl3d::Texture occlusionTexture = {}, gl3d::Texture emmisiveTexture = {});

		Material createMaterial(Material m);

		std::vector<Material> loadMaterial(std::string file, int quality);

		bool deleteMaterial(Material m);  
		bool copyMaterialData(Material dest, Material source);

		MaterialValues getMaterialValues(Material m);
		void setMaterialValues(Material m, MaterialValues values);

		std::string getMaterialName(Material m);
		void setMaterialName(Material m, const std::string& name);
		
		TextureDataForMaterial getMaterialTextures(Material m);
		void setMaterialTextures(Material m, TextureDataForMaterial textures);

		bool isMaterial(Material& m);

		//returns true if succeded
		//bool setMaterialData(Material m, const MaterialValues &data, std::string *s = nullptr);

	#pragma endregion

	#pragma region Texture

		Texture loadTexture(std::string path, int quality);
		Texture loadTextureFromMemory(objl::LoadedTexture &t, int quality);
		GLuint getTextureOpenglId(Texture& t);
		bool isTexture(Texture& t);

		void deleteTexture(Texture& t);

		GpuTexture* getTextureData(Texture& t);

		//internal
		Texture createIntenralTexture(GpuTexture t, int alphaData, int alphaValues, const std::string &name = "");
		Texture createIntenralTexture(GLuint id_, int alphaData, int alphaValues, const std::string &name = "");

		PBRTexture createPBRTexture(Texture& roughness, Texture& metallic,
			Texture& ambientOcclusion, int quality);
		void deletePBRTexture(PBRTexture &t);

	#pragma endregion

	#pragma region skyBox

		SkyBox loadSkyBox(const char* names[6]);
		SkyBox loadSkyBox(const char* name, int format = 0);
		SkyBox loadHDRSkyBox(const char* name);
		void deleteSkyBoxTextures(SkyBox& skyBox);

		SkyBox atmosfericScattering(glm::vec3 sun, glm::vec3 color1, glm::vec3 color2,
			glm::vec3 groundColor, bool useGroundColor, float g);
		SkyBox atmosfericScattering(AtmosfericScatteringSettings settings);

	#pragma endregion

	#pragma region model

		//todo implement stuff here

		//position 3, normal 3, optional texture 2
		Model createModelFromData(Material material, std::string name, 
			size_t vertexCount, const float *vertices, size_t indexesCount = 0,
			const unsigned int *indexes = nullptr, bool noTexture = false);

		Model loadModel(std::string path, int quality, float scale);
		bool isModel(Model& m);
		void deleteModel(Model &m);

		void clearModelData(Model& m);
		int getModelMeshesCount(Model& m);
		std::string getModelMeshesName(Model& m, int index);

		//for apis like imgui
		std::vector<char*>* getModelMeshesNames(Model& m);

	#pragma endregion
	
	#pragma region point lights
		PointLight createPointLight(glm::vec3 position, glm::vec3 color = glm::vec3(1.f),
			float dist = 20, float attenuation = 1);
		void detletePointLight(PointLight& l);

		glm::vec3 getPointLightPosition(PointLight& l);
		void setPointLightPosition(PointLight& l, glm::vec3 position);
		bool isPointLight(PointLight& l);
		glm::vec3 getPointLightColor(PointLight& l);
		void setPointLightColor(PointLight& l, glm::vec3 color);
		float getPointLightDistance(PointLight& l); //light distance
		void setPointLightDistance(PointLight& l, float distance); //light distance
		float getPointLightAttenuation(PointLight& l); //light distance
		void setPointLightAttenuation(PointLight& l, float attenuation); //light distance
		bool getPointLightShadows(PointLight& l);
		void setPointLightShadows(PointLight& l, bool castShadows = true);
		float getPointLightHardness(PointLight& l);
		void setPointLightHardness(PointLight& l, float hardness);
		int getPointLightShadowSize();
		void setPointLightShadowSize(int size);

	#pragma endregion

	#pragma region directional light

		DirectionalLight createDirectionalLight(glm::vec3 direction, 
			glm::vec3 color = glm::vec3(1.f), float hardness = 1, bool castShadows = 1);
		void deleteDirectionalLight(DirectionalLight& l);
		bool isDirectionalLight(DirectionalLight& l);
		
		glm::vec3 getDirectionalLightDirection(DirectionalLight& l);
		void setDirectionalLightDirection(DirectionalLight& l, glm::vec3 direction);

		glm::vec3 getDirectionalLightColor(DirectionalLight& l);
		void setDirectionalLightColor(DirectionalLight& l, glm::vec3 color);

		float getDirectionalLightHardness(DirectionalLight& l);
		void setDirectionalLightHardness(DirectionalLight& l, float hardness);
		
		bool getDirectionalLightShadows(DirectionalLight& l);
		void setDirectionalLightShadows(DirectionalLight& l, bool castShadows);
		
		int getDirectionalLightShadowSize();
		void setDirectionalLightShadowSize(int size);

	#pragma endregion

	#pragma region spot light

		SpotLight createSpotLight(glm::vec3 position, float fovRadians,
			glm::vec3 direction, float dist = 20, float attenuation = 1, 
			glm::vec3 color = glm::vec3(1), float hardness = 1, int castShadows = 1);

		//angles is the angle from zenith and azimuth
		SpotLight createSpotLight(glm::vec3 position, float fovRadians,
			glm::vec2 anglesRadians, float dist = 20, float attenuation = 1,
			glm::vec3 color = glm::vec3(1), float hardness = 1, int castShadows = 1);

		void deleteSpotLight(SpotLight& l);

		glm::vec3 getSpotLightPosition(SpotLight& l);
		void setSpotLightPosition(SpotLight& l, glm::vec3 position);
		bool isSpotLight(SpotLight& l);
		glm::vec3 getSpotLightColor(SpotLight& l);
		void setSpotLightColor(SpotLight& l, glm::vec3 color);
		float getSpotLightFov(SpotLight& l);
		void setSpotLightFov(SpotLight& l, float fovRadians);
		glm::vec3 getSpotLightDirection(SpotLight& l);
		void setSpotLightDirection(SpotLight& l, glm::vec3 direction);
		float getSpotLightDistance(SpotLight& l); //light distance
		void setSpotLightDistance(SpotLight& l, float distance); //light distance
		float getSpotLightAttenuation(SpotLight& l); //light distance
		void setSpotLightAttenuation(SpotLight& l, float attenuation); //light distance
		float getSpotLightHardness(SpotLight& l);
		void setSpotLightHardness(SpotLight& l, float hardness);
		void setSpotLightShadows(SpotLight& l, bool castShadows);
		bool getSpotLightShadows(SpotLight& l);
		int getSpotLightShadowSize();
		void setSpotLightShadowSize(int size);

	#pragma endregion

	#pragma region Entity
	
		Entity createEntity(Model m, Transform transform = {}, 
			bool staticGeometry = 1, bool visible = 1, bool castShadows = 1);

		Entity duplicateEntity(Entity &e);

		void setEntityModel(Entity& e, Model m);
		void clearEntityModel(Entity& e);
		CpuEntity* getEntityData(Entity &e); //todo this will probably dissapear
		Transform getEntityTransform(Entity &e);
		void setEntityTransform(Entity &e, Transform transform);
		bool isEntityStatic(Entity &e);
		void setEntityStatic(Entity &e, bool s = true);
		void deleteEntity(Entity& e);
		bool isEntity(Entity& e);
		bool isEntityVisible(Entity& e);
		void setEntityVisible(Entity& e, bool v = true);
		void setEntityCastShadows(Entity& e, bool s = true);
		bool getEntityCastShadows(Entity& e);
		void setEntityAnimationIndex(Entity &e, int ind);

		//transitions from to ecurrent state
		//to a new animation specified newAnimationIndex. it takes transitionTimeSecconds,
		//and it will translate to newAnimationIndex at newAnimationTimeStampSecconds time stamp.
		//if the object is not curently animated it will start the animation.
		void transitionToAnimation(Entity& e, int newAnimationIndex, float transitionTimeSecconds,
			float newAnimationTimeStampSecconds = 0);


		int getEntityAnimationIndex(Entity &e);
		void setEntityAnimationSpeed(Entity &e, float speed);
		float getEntityAnimationSpeed(Entity &e);
		void setEntityAnimate(Entity& e, bool animate); //true = display animations. To pause set the animation speed to 0.
		bool getEntityAnimate(Entity& e); //returns true if it is animating
		bool entityCanAnimate(Entity& e); //returns true if it is can be animated
		int getEntityJointIndex(Entity &e, std::string name); //returns -1 on error

		bool getEntityJointTransform(Entity &e, std::string boneName, Transform &t); //returns 0 on error
		bool getEntityJointTransform(Entity &e, int boneIndex, Transform &t); //returns 0 on error

		//this is used for apis like imgui.
		std::vector<char*> *getEntityMeshesNames(Entity& e);

		int getEntityMeshesCount(Entity& e);
		MaterialValues getEntityMeshMaterialValues(Entity& e, int meshIndex);
		void setEntityMeshMaterialValues(Entity& e, int meshIndex, MaterialValues mat);

		std::string getEntityMeshMaterialName(Entity& e, int meshIndex);
		void setEntityMeshMaterialName(Entity& e, int meshIndex, const std::string &name);
		
		void setEntityMeshMaterial(Entity& e, int meshIndex, Material mat);
		
		TextureDataForMaterial getEntityMeshMaterialTextures(Entity& e, int meshIndex);
		void setEntityMeshMaterialTextures(Entity& e, int meshIndex, TextureDataForMaterial texture);

	#pragma endregion

	#pragma region settings


		void setExposure(float exposure);
		float getExposure();

		//cheap
		void enableNormalMapping(bool normalMapping = 1);
		bool isNormalMappingEnabeled();

		//cheap (will calculate the light from sky box more accurately)
		void enableLightSubScattering(bool lightSubScatter = 1);
		bool isLightSubScatteringEnabeled();

		//rather expensive
		void enableSSAO(bool ssao = 1);
		bool &isSSAOenabeled();
		float &getSSAOBias();
		void setSSAOBias(float bias);
		float &getSSAORadius();
		void setSSAORadius(float radius);
		int &getSSAOSampleCount();
		void setSSAOSampleCount(int samples);
		float &getSSAOExponent();
		void setSSAOExponent(float exponent);
		
		bool &colorCorrection();
		//the memory of the lookup texture is not managed by the engine and not cleared by the engine on clearAll.
		ColorLookupTexture &colorCorrectionTexture();


		//bloom
		//more or less expensive
		//this is setter and getter
		bool &bloom();
		float getBloomTresshold();
		void setBloomTresshold(float b);
		void setBloomIntensisy(float b);
		bool &bloomHighQualityDownSample();
		bool &bloomHighQualityUpSample();
		
		//SSR
		LightShader::LightPassData::SSRdata &getSSRdata();
		void setSSRdata(LightShader::LightPassData::SSRdata data);
		void enableSSR(bool enable = true);
		bool isSSRenabeled();

		//
		float &getDirectionalShadowCascadesFrustumSplit(int cascadeIndex);

		//chromatic aberation
		bool &chromaticAberationEnabeled();
		//in pixels
		float getChromaticAberationStrength();
		//in pixels
		void setChromaticAberationStrength(float pixels);
		float getChromaticAberationUnfocusDistance();
		void setChromaticAberationUnfocusDistance(float distance);
		
		#pragma region fxaa

			//todo explain this parameters
			//http://blog.simonrodriguez.fr/articles/2016/07/implementing_fxaa.html
			struct FXAAData
			{
				float edgeMinTreshold = 0.028;
				float edgeDarkTreshold = 0.125;
				int ITERATIONS = 12;
				float quaityMultiplier = 0.8;
				float SUBPIXEL_QUALITY = 0.95;
			};

			//very little performance penalty
			void enableFXAA(bool fxaa = 1);
			FXAAData& getFxaaSettings();
			bool& isFXAAenabeled();

		#pragma endregion
		
		//saves the current settings to a string;
		std::string saveSettingsToJson(bool includeRenderingSettings, std::string skyBoxName = "", 
			gl3d::AtmosfericScatteringSettings *atmosphericScattering = nullptr);

		//this will terminate if data is not a valid json
		void loadSettingsFromJson(const char *data, bool includeRenderingSettings, bool loadSkyBox, bool loadAtmosphericScattering);


	#pragma endregion
			

		Camera camera;
		SkyBox skyBox;

		int tonemapper = 1; //todo add enum, todo save data

		//debug stuff todo
		void renderModelNormals(Model o, glm::vec3 position, glm::vec3 rotation = {},
			glm::vec3 scale = { 1,1,1 }, float normalSize = 0.5, glm::vec3 normalColor = {0.7, 0.7, 0.1});
		void renderSubModelNormals(Model o, int index, glm::vec3 position, glm::vec3 rotation = {},
			glm::vec3 scale = { 1,1,1 }, float normalSize = 0.5, glm::vec3 normalColor = { 0.7, 0.7, 0.1 });

		void renderSubModelBorder(Model o, int index, glm::vec3 position, glm::vec3 rotation = {},
			glm::vec3 scale = { 1,1,1 }, float borderSize = 0.5, glm::vec3 borderColor = { 0.7, 0.7, 0.1 });

		struct ColorCorrection
		{
			GLuint u_texture;
			GLuint u_lookup;

			Shader shader;

			GLuint fbo;
			//fbo texture
			GLuint texture;

			glm::ivec2 currentDimensions = {};

			bool colorCorrection = 0; //todo also check if texture is present

			void create(int w, int h, ErrorReporter &errorReporter, FileOpener &fileOpener);

			void resize(int w, int h);

			void clear();

			//the memory of the lookup texture is not managed by the engine and not cleared by the engine on clearAll.
			ColorLookupTexture currentTexture; //the texture supplied by user

		};

		struct InternalStruct
		{
			Camera lastFrameCamera;
			LightShader lightShader;

			int w; int h;
			int adaptiveW; int adaptiveH;

			struct PBRtextureMaker
			{
				Shader shader;
				GLuint fbo;

				void init(ErrorReporter &errorReporter, FileOpener &fileOpener);

				GLuint createRMAtexture(
					GpuTexture roughness, GpuTexture metallic, GpuTexture ambientOcclusion, 
					GLuint quadVAO, int &RMA_loadedTextures, GLuint frameBuffer, int quality);
				
				void clear();

			}pBRtextureMaker;

			SkyBoxLoaderAndDrawer skyBoxLoaderAndDrawer;
			void renderSkyBox(Camera &c, SkyBox &s); //todo remove this later
			void renderSkyBoxBefore(Camera& c, SkyBox& s);

			int getMaterialIndex(Material &m);
			int getModelIndex(Model &o);
			int getTextureIndex(Texture &t);
			int getEntityIndex(Entity &e);
			int getSpotLightIndex(SpotLight &l);
			int getPointLightIndex(PointLight &l);
			int getDirectionalLightIndex(DirectionalLight &l);

			//material
			std::vector<MaterialValues> materials;
			std::vector<int> materialIndexes;
			std::vector<std::string> materialNames;
			std::vector<TextureDataForMaterial> materialTexturesData;

			bool getMaterialData(Material m, MaterialValues* gpuMaterial,
				std::string* name, TextureDataForMaterial* textureData);

			//texture
			std::vector <internal::GpuTextureWithFlags> loadedTextures;
			std::vector<GLuint64> loadedTexturesBindlessHandle;
			std::vector<int> loadedTexturesIndexes;
			std::vector<std::string> loadedTexturesNames;

			//models
			std::vector< ModelData > graphicModels;
			std::vector<int> graphicModelsIndexes;

			ModelData* getModelData(Model o);

			//entities
			std::vector<CpuEntity> cpuEntities;
			std::vector<int> entitiesIndexes;

			//spot lights
			std::vector<internal::GpuSpotLight> spotLights;
			std::vector<int> spotLightIndexes;

			//point lights
			std::vector<internal::GpuPointLight> pointLights;
			std::vector<int> pointLightIndexes;

			//directional lights
			std::vector<internal::GpuDirectionalLight> directionalLights;
			std::vector<int> directionalLightIndexes;


			struct PerFrameFlags
			{
				bool staticGeometryChanged = 0;
				bool shouldUpdateSpotShadows = 0;
				bool shouldUpdatePointShadows = 0;
				bool shouldUpdateDirectionalShadows = 0;

			}perFrameFlags;
			
			#pragma region different shaders

			struct
			{
				Shader shader;
				GLint modelTransformLocation;
				GLint projectionLocation;
				GLint sizeLocation;
				GLint colorLocation;
				
				//todo add a create function when ill work at this
			}showNormalsProgram;

			struct SSAO
			{
				//https://learnopengl.com/Advanced-Lighting/SSAO

				void create(int w, int h, ErrorReporter &errorReporter, FileOpener &fileOpener, GLuint frameBuffer);
				void resize(int w, int h);

				void clear();

				glm::ivec2 currentDimensions = {};

				GLuint noiseTexture;
				GLuint ssaoFBO;
				GLuint ssaoColorBuffer;

				Shader shader;

				GLuint ssaoUniformBlockBuffer;
				struct SsaoShaderUniformBlockData
				{
					float radius = 0.2;
					float bias = 0.025;
					int samplesTestSize = 16; // should be less than kernelSize (64)

				}ssaoShaderUniformBlockData;

				GLint u_projection = -1;
				GLint u_view = -1;
				GLint u_gPosition = -1;
				GLint u_gNormal = -1;
				GLint u_texNoise = -1;
				GLint u_samples = -1;
				GLuint u_SSAODATA;


				std::vector<glm::vec3> ssaoKernel;

				GLuint blurBuffer;
				GLuint blurColorBuffer;
				GLint u_ssaoInput;
				Shader blurShader;

				float ssao_finalColor_exponent = 5.f;
			}ssao;

			struct HBAO
			{
				//https://developer.download.nvidia.com/presentations/2008/SIGGRAPH/HBAO_SIG08b.pdf

				void create(ErrorReporter &errorReporter, FileOpener &fileOpener);
				void clear();
				Shader shader;

				GLint u_projection = -1;
				GLint u_view = -1;
				GLint u_gPosition = -1;
				GLint u_gNormal = -1;
				GLint u_texNoise = -1;

			}hbao;

			struct GBuffer
			{
				void create(int w, int h, ErrorReporter &errorReporter, GLuint frameBuffer);
				void resize(int w, int h);

				void clear();

				enum bufferTargers
				{
					normal = 0,
					textureDerivates,
					//albedo,
					//material,
					positionViewSpace,
					//emissive,
					materialIndex,
					textureUV,
					//textureDerivates,
					bufferCount,
				};

				unsigned int gBuffer;
				unsigned int buffers[bufferCount];
				unsigned int depthBuffer;

				glm::ivec2 currentDimensions = {};

			}gBuffer;

			#pragma endregion

			bool hasLastFrameTexture = 1;

			ColorCorrection colorCorrection;

		}internal;
		

		struct PostProcess
		{
			struct
			{
				Shader shader;
				GLint u_texture;
				GLint u_exposure;
				GLint u_tresshold;
				GLint u_tonemapper;
			}filterShader;

			struct
			{
				Shader shader;
				GLint u_texture;
				GLint u_mip;
			}addMips;

			struct
			{
				Shader shader;
				GLint u_texture;
				GLint u_mip;
			}addMipsBlur;

			struct
			{
				Shader shader;
				GLint u_texture;
				GLint u_mip;
			}filterDown;

			struct
			{
				Shader shader;
				GLint u_finalColorTexture;
				GLint u_windowSize;
				GLint u_strength;
				GLint u_DepthTexture;
				GLint u_near;
				GLint u_far;
				GLint u_unfocusDistance;
				GLuint fbo;
			}chromaticAberation;

			Shader postProcessShader;
			Shader gausianBLurShader;
			GLint u_colorTexture;	//post process shader
			GLint u_bloomTexture;	//post process shader
			GLint u_bloomNotBluredTexture;	//post process shader
			GLint u_bloomIntensity;	//post process shader
			GLint u_exposure;		//post process shader
			GLint u_useSSAO;	//post process shader
			GLint u_ssaoExponent;	//post process shader
			GLint u_ssao;	//post process shader
			GLint u_tonemapper; //post process shader

			GLint u_toBlurcolorInput;
			GLint u_horizontal;
			GLint u_mip;
			GLint u_texel;

			GLuint fbo;
			GLuint filterFbo;
			GLuint blurFbo[2];

			GLuint colorBuffers[3]; // 0 for color, 1 for bloom, 2 last frame
			GLuint bluredColorBuffer[2];
			void create(int w, int h, ErrorReporter &errorReporter, FileOpener &fileOpener, GLuint frameBuffer);
			void resize(int w, int h);
			void clear();
			glm::ivec2 currentDimensions = {};
			int currentMips = 1;

			//for post process shader
			float bloomIntensty = 1;

			bool highQualityDownSample = 0;
			bool highQualityUpSample = 0;

			bool chromaticAberationOn = 0;
			float chromaticAberationStrength = 20.f;
			float unfocusDistance = 5.f;

		}postProcess;

		//used for adaptive resolution or fxaa or both
		struct AdaptiveResolution
		{
			void create(int w, int h);
			void resize(int w, int h);
			void clear();

			static constexpr int timeSamplesCount = 20;
			float msSampled[timeSamplesCount] = {};
			int timeSample = 0;

			float stepDownSecTarget = 17.f;
			float stepUpSecTarget = 12.f;

			glm::ivec2 currentDimensions = {};
			float rezRatio = 1.f;
			float maxScaleDown = 0.6;
			bool useAdaptiveResolution = true;
			bool shouldUseAdaptiveResolution = false;

			GLuint texture;
			GLuint fbo;

			GLuint texture2;
			GLuint fbo2;

		}adaptiveResolution;

		struct AntiAlias
		{
			Shader shader;
			Shader noAAshader;
			void create(ErrorReporter &errorReporter, FileOpener &fileOpener);
			void clear();

			GLuint u_texture;
			GLuint noAAu_texture;
			GLuint u_FXAAData;
			GLuint fxaaDataBuffer;

			FXAAData fxaaData;

			bool usingFXAA = true;
		}antiAlias;

		ColorLookupTexture loadColorLookupTextureFromFile(const char *path);

		struct CopyDepth
		{
			gl3d::Shader shader;
			GLint u_depth = -1;

			void create(ErrorReporter &errorReporter, FileOpener &fileOpener);
			void clear();
		}copyDepth;

		struct DirectionalShadows
		{
			void create(GLuint frameBuffer);
			void allocateTextures(int count);
			void clear();

			constexpr static int CASCADES = 3;

			int textureCount = 0;

			GLuint cascadesTexture;
			GLuint staticGeometryTexture;
			GLuint cascadesFbo;
			GLuint staticGeometryFbo;
			int shadowSize = 2048;
			int currentShadowSize = 2048;

			float frustumSplits[CASCADES] = { 0.01,0.03,0.1 };


		}directionalShadows;

		struct SpotShadows
		{
			void create(GLuint frameBuffer);
			void allocateTextures(int count);
			void clear();
			int textureCount = 0;

			GLuint shadowTextures;
			GLuint staticGeometryTextures;
			GLuint fbo;
			GLuint staticGeometryfbo;

			int shadowSize = 1024;
			int currentShadowSize = 1024;

		}spotShadows;

		struct PointShadows
		{
			void create(GLuint frameBuffer);
			void allocateTextures(int count);
			void clear();
			int textureCount = 0;

			int shadowSize = 1024;
			int currentShadowSize = 1024;


			GLuint shadowTextures;
			GLuint staticGeometryTextures;
			GLuint fbo;
			GLuint staticGeometryFbo;

		}pointShadows;

		void render(float deltaTime);

		void updateWindowMetrics(int x, int y);

		void clearAllLoadedResource();

		//only cleares the current skybox!, other user created skyboxes won't be cleared
		void clearAllRendererResources();

		bool frustumCulling = 1;
		bool zPrePass = 0;
		bool copyDepthForLaterForwardRendering = 0;

	};




};