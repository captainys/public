#ifndef FSVULKAN_IS_INCLUDED
#define FSVULKAN_IS_INCLUDED
/* { */



#include "vulkan/vulkan.h"
#include "vulkan/vk_sdk_platform.h"

#include "fsvulkandef.h"

#define FsVulkan_INVALID_INDEX 0x7fffffff


#ifdef __cplusplus
#include <vector>

class FsVulkanContext
{
private:
	static FsVulkanContext context;

	// Here's one of the design failures of Vulkan.  How can I know how many samplers I use beforehand?
	// No!  Absolutely no way!
	// And Vulkan requires the pool to be created with maximum size?
	// In the name of performance?
	// Absurd!
	enum
	{
		UNIFORM_POOL_SIZE=1024*1024,
	};

public:
	static FsVulkanContext &GetContext(void);

	class Buffer // Question: Do I have to separate vertex buffer and uniform buffer? -> Yes, I do need to.  Uniform is a pain in the neck of Vulkan.
	{
	public:
		VkBuffer buf;
		VkDeviceMemory mem;
		long long int length;
		inline Buffer()
		{
			buf=VK_NULL_HANDLE;
			mem=VK_NULL_HANDLE;
		}
	};
	class Texture2d
	{
	public:
		VkImage image;
		VkImageView imageView;
		VkSampler sampler;
		VkDeviceMemory mem;

		Texture2d();
		void CleanUp(VkDevice logicalDevice);
	};

	class DescriptorSetAndPool
	{
	public:
		VkDescriptorPool descPool;
		VkDescriptorSet descSet;
	};
	class Uniform : public DescriptorSetAndPool
	{
	public:
		long long int length;
	};
	class Sampler : public DescriptorSetAndPool
	{
	public:
	};


	class OneTimeCommandGuard
	{
	friend class FsVulkanContext;
	private:
		VkCommandBuffer cmdBuf;
		const FsVulkanContext *contextPtr;
	public:
		OneTimeCommandGuard(const FsVulkanContext &context);
		~OneTimeCommandGuard();
		inline operator VkCommandBuffer()
		{
			return cmdBuf;
		}
	};


	class Renderer;
	class RenderTarget;

	class Frame
	{
	friend class FsVulkanContext;

	public:

	private:
		RenderTarget *owner;
		std::vector <Buffer> perFrameBuffer;
		VkDescriptorPool perFrameUniformPool;
	public:
		uint32_t imageIndex;
		VkExtent2D imageExtent;
		VkImage image;
		VkImageView imageView;
		VkFramebuffer framebuffer;
		VkCommandBuffer commandBuffer;

		VkImage depthImage;
		VkDeviceMemory depthMemory;
		VkImageView depthImageView;

		Frame()
		{
			image=VK_NULL_HANDLE;
			imageView=VK_NULL_HANDLE;
			framebuffer=VK_NULL_HANDLE;
			commandBuffer=VK_NULL_HANDLE;
			perFrameUniformPool=VK_NULL_HANDLE;

			depthImage=VK_NULL_HANDLE;
			depthMemory=VK_NULL_HANDLE;
			depthImageView=VK_NULL_HANDLE;
		}

	public:
		/*! No matter how it is inefficient, there are some primitives that change every frame,
		    that even changes the length,
		    or that I don't want to keep a permanent resource for (where I want to be sloppy).
		    For those primitives, use a per-frame buffer.
		*/
		Buffer CreatePerFrameVertexBuffer(long long int bufSize,const void *data=nullptr);

		/*! No matter how it is inefficient, there are many uniforms that change every frame.
		    For those uniforms, use a per-frame uniform.
		    Per-frame uniforms will be destroyed when the next frame is ready (uniforms are no longer used).
		*/
		Buffer CreatePerFrameUniformBuffer(long long int bufSize,const void *data=nullptr);

		/*! Create a uniform that is ready to bind with vkCmdBindDescriptorSets.
		    The uniform is owned by the frame, and will be erased (freed) when the next rendering 
		    of this frame begins.
		    Uniform index (uniformIdx) is not a binding.  It is the index to uniformLayout member of the Renderer.
		*/
		VkDescriptorSet CreatePerFrameUniformBinder(const Renderer &renderer,int uniformIdx,Buffer uniformBuf);

		/*! Create a uniform that is ready to bind with vkCmdBindDescriptorSets.
		    The uniform is owned by the frame, and will be erased (freed) when the next rendering 
		    of this frame begins.
		    Uniform index (uniformIdx) is not a binding.  It is the index to uniformLayout member of the Renderer.
		*/
		VkDescriptorSet CreatePerFrameUniformBinder(const Renderer &renderer,int uniformIdx,long long int bufSize,const void *data);

		/*! Create a sampler that is ready to bind with vkCmdBindDescriptorSets.
		    The sample is owned by the frame, and will be erased (freed) when the next rendering 
		    of this frame begins.
		    Uniform index (uniformIdx) is not a binding.  It is the index to uniformLayout member of the Renderer.

		    If the renderer is not made in the way that the index is different from binding, it would be very
		    confusing.  Therefore, the renderer should be designed so that the index matches the binding.
		*/
		VkDescriptorSet CreatePerFrameSamplerBinder(const Renderer &renderer,int uniformIdx,const Texture2d &tex2d) const;

		/*! Create a sampler that is ready to bind with vkCmdBindDescriptorSets.
		    The sample is owned by the frame, and will be erased (freed) when the next rendering 
		    of this frame begins.
		    Uniform index (uniformIdx) is not a binding.  It is the index to uniformLayout member of the Renderer.

		    If the renderer is not made in the way that the index is different from binding, it would be very
		    confusing.  Therefore, the renderer should be designed so that the index matches the binding.
		*/
		VkDescriptorSet CreatePerFrameSamplerBinder(const Renderer &renderer,int uniformIdx,long long int nTex,const Texture2d tex2d[]) const;


		/*! Start command-buffer recording.
		    Since it implicitly resets the command buffer, nothing should be referring to
		    the per-frame (temporary) resources.
		    Therefore it also destroys per-frame (temporary) Vulkan resources.
		*/
		void BeginCommandBuffer(void);


		/*! Destroy all per-frame (or on-the-fly) resources.
		    Resource handles will be nullified in this function, therefore calling this function multiple times
		    will not crash the program.
		*/
		void DestroyAllPerFrameResource(void);
	};


	class RendererPool;

	class Renderer
	{
	friend class FsVulkanContext;
	public:
		enum VTATTRIBTYPE
		{
			VERTEX2F,
			VERTEX3F,
			NORMAL3F,
			COLOR4F,
			TEXCOORD2F,
			POINTSIZE1F,
		};
		class UniformBinding
		{
		public:
			VkDescriptorSetLayout layout;
			uint32_t binding;
			long long int length;
		};
	private:
		RendererPool *owner;
		std::vector <UniformBinding> uniformBinding;
		std::vector <VTATTRIBTYPE> attribBinding;
		void AddUniformBinding(uint32_t binding,VkDescriptorSetLayout layout,long long int length);
	public:
		VkPipelineLayout pipelineLayout;
		VkPipeline pointListPipeline,triangleListPipeline,lineListPipeline;

		Renderer(RendererPool *targetPtr);
		virtual ~Renderer(){CleanUp();}

		/*! Get uniform (can be a sampler) layout.
		*/
		UniformBinding GetUniformBinding(int idx) const;

		/*! Get vertex binding. */
		const std::vector <VTATTRIBTYPE> &GetAttribBinding(void) const;

		/*! Destroys resources.
		*/
		virtual void CleanUp(void);
	};
	class Plain2dRenderer : public Renderer
	{
	friend class FsVulkanContext;
	public:
		Plain2dRenderer(RendererPool *owner) : Renderer(owner) {}

		/*! Reset model transformation.
		    Transformations are push constants.  Therefore it needs to be reset again if
		    push constants were modified for other pipelines.
			This also affects Texture2d*Renderer.
		*/
		void CmdResetModelTransformation(Frame &frame);

		/*! Set up window transformation to use pixel coordinate.
		    Transformations are push constants.  Therefore it needs to be reset again if
		    push constants were modified for other pipelines.
			This also affects Texture2d*Renderer.
		*/
		void CmdUsePixelCoordTopLeftAsOrigin(Frame &frame);

		/*! Set up window transformation to use normalized window coordinate.
		    Transformations are push constants.  Therefore it needs to be reset again if
		    push constants were modified for other pipelines.
			This also affects Texture2d*Renderer.
		*/
		void CmdUseNormalizedCoordTopLeftAsOrigin(Frame &frame);

		/*! Specify window transformation.

			{x'}={t[0] t[2] t[4]}{x}
			{y'}={t[1] t[3] t[5]}{y}
			                     {1}

		    Transformations are push constants.  Therefore it needs to be reset again if
		    push constants were modified for other pipelines.
			This also affects Texture2d*Renderer.
		*/
		void CmdSetWindowTransformation(Frame &frame,const float t[6]);

		/*! Specify model transformation.

			{x'}={t[0] t[2] t[4]}{x}
			{y'}={t[1] t[3] t[5]}{y}
			                     {1}
		    Transformations are push constants.  Therefore it needs to be reset again if
		    push constants were modified for other pipelines.
			This also affects Texture2d*Renderer.
		*/
		void CmdSetModelTransformation(Frame &frame,const float t[6]);
	};
	class Texture2dRenderer : public Plain2dRenderer
	{
	friend class FsVulkanContext;
	public:
		Texture2dRenderer(RendererPool *owner) : Plain2dRenderer(owner){}
	};


	class ExperimentQuad2dRenderer : public Plain2dRenderer
	{
	friend class FsVulkanContext;
	typedef ExperimentQuad2dRenderer THISCLASS;
	public:
		ExperimentQuad2dRenderer(RendererPool *owner) : Plain2dRenderer(owner){}

		struct State
		{
			float vertex[16];
			float color[16];
		};
		State *statePtr;
		Buffer stateUniform;

		void CleanUp(void);
		VkDescriptorSet CreateCurrentStateUniformBinder(Frame &frame) const;
	};


	class Renderer3dBase : public Renderer
	{
	public:
		inline Renderer3dBase(RendererPool *owner) : Renderer(owner) {}

		/*! Upload modelView matrix to the push constants.
		*/
		void CmdSetModelView(Frame &frame,const float tfm[16]);
	};
	class Plain3dRenderer : public Renderer3dBase
	{
	friend class FsVulkanContext;
	public:
		Plain3dRenderer(RendererPool *owner) : Renderer3dBase(owner) {}
	};

	class Shaded3dRenderer : public Renderer3dBase
	{
	friend class FsVulkanContext;
	public:
		struct State
		{
			float projection[16];

			float lightEnabled[YSVULKAN_MAX_NUM_LIGHT];
			float lightPos[YSVULKAN_MAX_NUM_LIGHT][4];
			float lightColor[YSVULKAN_MAX_NUM_LIGHT][4];  // 3D
			float ambientColor[3],_padding0;
			float specularColor[3],_padding1;
			float specularExponent;

			float alphaCutOff;

			float fogEnabled;
			float fogDensity;
			float fogColor[4];

			float zOffset;
			float _padding4;

			uint32_t textureType;
			float useTexture;
			float textureSampleCoeff[4]; // 3D

			float textureTileTransform[16];
			uint32_t billBoardTransformType;
			uint32_t useNegativeOneToPositiveOneTexCoord;
			float _padding5[2];
			float billBoardCenter[3],_padding2;
			float billBoardDimension[2],_padding3[2];

			float useShadowMap[YSVULKAN_SHADOWMAP_MAX_NUM_TEXTURE];
			float shadowMapTransform[YSVULKAN_SHADOWMAP_MAX_NUM_TEXTURE][16];
			float lightDistScale[YSVULKAN_SHADOWMAP_MAX_NUM_TEXTURE];
			float lightDistOffset[YSVULKAN_SHADOWMAP_MAX_NUM_TEXTURE];
		};
		State currentState;

		Shaded3dRenderer(RendererPool *owner) : Renderer3dBase(owner) {currentState={};}

		/*! Create per-frame uniform.  
		   (Efficiency?  Shut up!  You graphics APIs are too arrogant.  Graphics API must not invade the core data structure!)
		*/
		VkDescriptorSet CreateCurrentStateUniformBinder(Frame &frame) const;
	};

	class PointSprite3dRenderer : public Renderer3dBase
	{
	friend class FsVulkanContext;
	public:
		PointSprite3dRenderer(RendererPool *owner);

		struct State
		{
			float projection[16];

			float viewportWid;			// 16-byte alignment
			float viewportHei;

			float alphaCutOff;

			float zOffset;

			float fogColor[4];			// 16-byte alignment
			float fogEnabled;			// 16-byte alignment
			float fogDensity;

			int32_t textureType;
			float useTexture;

			float usePointSizeInPixel;	// 16-byte alignment
			float usePointSizeIn3D;
			float usePointClippingCircle;
			float texCoordRange;
		};
		State currentState;

		/*! Create per-frame uniform.  
		   (Efficiency?  Shut up!  You graphics APIs are too arrogant.  Graphics API must not invade the core data structure!)
		*/
		VkDescriptorSet CreateCurrentStateUniformBinder(Frame &frame) const;
	};


	class RenderPassBeginInfo
	{
	public:
		VkRenderPassBeginInfo beginInfo;
		VkClearValue clearColor[2];
	};


	class RenderTarget
	{
	public:
		FsVulkanContext *owner;

	public:
		std::vector <Frame> frame;
		VkRenderPass renderPass;
		VkFormat imageFormat;

		void CreateRenderPass(void);
		void CreateFramebuffer(void);
		void CreateUniformPool(void);
		void CreateDepthBuffer(void);
		void CreateCommandBuffer(void);


		/*! Destroys the render pass and associated Vulkan resources .
		*/
		void DestroyRenderTarget(void);

		// Future: want to make a function like:
		// template <class RendererClass,class RendererOption>
		// Renderer *CreateRenderer(PrimitiveType primType,RendererOption opt);
	};
	class SwapChain : public RenderTarget
	{
	public:
		VkSwapchainKHR swapChain;

		void CreateSwapChain(void);
		void DestroySwapChain(void);

		/*! Wait for the next frame to be ready.
		*/
		Frame &ReadyFrame(void);

		/*! Returns a set of information needed for starting a render pass.
		    If only a sub-region of the image needs to be drawn, the values can be
		    costomized before given to vkCmdBeginRenderPass.
		*/
		RenderPassBeginInfo MakeRenderPassBeginInfo(const Frame &frame) const;
	};
	class RenderToTexture : public RenderTarget
	{
	public:
		VkSampler sampler;
		VkSemaphore renderFinishedSemaphore;
		bool hasColor,hasDepth;

		RenderToTexture();

		/*! Creates a shadow map.  A RenderToTexture can have multiple shadow maps (frame[0] ... frame[nShadowMap-1]).
		*/
		void CreateShadowMap(FsVulkanContext *owner,long long int nShadowMap,long long int wid,long long int hei);

		/*! Returns a set of information needed for starting a render pass.
		*/
		RenderPassBeginInfo MakeRenderPassBeginInfo(const Frame &frame) const;
	private:
		void CreateShadowMapRenderPass(long long int wid,long long int hei);
		void CreateShadowMapDepthBuffer(long long int nShadowMap,long long int wid,long long int hei);
		void CreateShadowMapSampler(long long int wid,long long int hei);

	public:
		void DestroyRenderToTexture(void);

		/*! Returns Texture2d with only imageView and sampler members filled.
		    The rest will be VK_NULL_HANDLE.
		    The imageView and sampler is owned by this class.  Therefore, whoever
		    receives a Texture2d with this function must never destroy the resources.
		*/
		Texture2d GetDepthTexture2d(long long int idx) const;
	};


	class RendererPool
	{
	public:
		FsVulkanContext *owner;

	private:
		std::vector <Renderer *> createdRenderer;

	public:
		RendererPool(FsVulkanContext *owner);


		/*! Destroys all render passes created for this render target.
		*/
		void DestroyRenderer(void);

		/*! Call RenderPassStarted function of the renderers.
		*/
		void CmdRenderPassStarted(Frame &frame);

		/*! Creates a plain-2d triangle renderer.
		    The renderer is owned by RenderPool.
		    Therefore it must not be destroyed outside of the RenderTarget class.
		    However, the renderer won't be destroyed during the re-creation of the render pass.
		    Therefore renderers DO NOT have to be re-created after the re-creation of the render pass.

		    The renderer must not be used with a renderPass that is not compatible with this given renderPass.
		    In other words, as long as the render pass is compatible, this Renderer can be used for a different render pass.
		*/
		Plain2dRenderer *CreatePlain2dTriangleRenderer(VkRenderPass renderPass);

		/*! Create a Texture2d renderer. 
		    The renderer is owned by RenderPool.
		    Therefore it must not be destroyed outside of the RenderTarget class.
		    However, the renderer won't be destroyed during the re-creation of the render pass.
		    Therefore renderers DO NOT have to be re-created after the re-creation of the render pass.
		*/
		Texture2dRenderer *CreateTexture2dTriangleRenderer(VkRenderPass renderPass);

		/*! Experimental renderer. */
		ExperimentQuad2dRenderer *CreateExperimentQuad2dRenderer(VkRenderPass renderPass);

		/*! Create a Plain3d renderer. 
		    The renderer is owned by RenderPool.
		    Therefore it must not be destroyed outside of the RenderTarget class.
		    However, the renderer won't be destroyed during the re-creation of the render pass.
		    Therefore renderers DO NOT have to be re-created after the re-creation of the render pass.
		*/
		Plain3dRenderer *CreatePlain3dRenderer(VkRenderPass renderPass);

		/*! Create a Shaded 3d triangle renderer. 
		    The renderer is owned by RenderPool.
		    Therefore it must not be destroyed outside of the RenderTarget class.
		    However, the renderer won't be destroyed during the re-creation of the render pass.
		    Therefore renderers DO NOT have to be re-created after the re-creation of the render pass.
		*/
		Shaded3dRenderer *CreateShaded3dRenderer(VkRenderPass renderPass);


		/*! Create a Point Sprite 3d renderer.
		    The renderer is owned by RenderPool.
		    Therefore it must not be destroyed outside of the RenderTarget class.
		    However, the renderer won't be destroyed during the re-creation of the render pass.
		    Therefore renderers DO NOT have to be re-created after the re-creation of the render pass.
		*/
		PointSprite3dRenderer *CreatePointSprite3dRenderer(VkRenderPass renderPass);
	};



private:
	class StockResource
	{
	public:
		Texture2d allOne2d,allZero2d,checker2d;

		void Create(FsVulkanContext &context);
		void CleanUp(FsVulkanContext &context);
	};
	StockResource stock;
public:
	/*! Returns a stocked texture that returns all ones (1,1,1,1) for any UV coordinate.
	*/
	Texture2d GetStockAllOneTexture2d(void) const;

	/*! Returns a stocked texture that returns all zeros (0,0,0,0) for any UV coordinate.
	*/
	Texture2d GetStockAllZeroTexture2d(void) const;

	/*! Returns a stocked texture that returns 8x8 checker pattern.
	*/
	Texture2d GetStockCheckerTexture2d(void) const;


	VkInstance inst;
	int useValidationLayer;
	VkSurfaceKHR surface;
	VkPhysicalDevice physicalDevice;
	VkDevice logicalDevice;
	VkCommandPool commandPool;
	bool sameQueue;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	VkDescriptorPool uniformPool;

	SwapChain primarySwapChain;
	RendererPool primaryRendererPool;

	VkSemaphore imageAvailableSemaphore,renderFinishedSemaphore;

	FsVulkanContext();
	~FsVulkanContext();
	void CleanUp(void);
	void CreateInstance(int useValidationLayer);
	void SetPlatformSpecificSurface(VkSurfaceKHR surface);
	void CreatePhysicalDevice(void);
	void CreateLogicalDevice(void); // Also creates command, uniform, and sampler pools.
	void CreateSemaphore(void);
	void CreateStockResoruce(void);

	VkShaderModule CreateShaderModule(long long int len,const unsigned char spirv[],const char shaderName[]="");

	void SubmitCommandBuffer(VkCommandBuffer cmdBuf,VkSemaphore waitSemaphore=VK_NULL_HANDLE,VkSemaphore signalSemaphore=VK_NULL_HANDLE);
	void SubmitCommandBuffer(const std::vector <VkCommandBuffer> &cmdBuf,VkSemaphore waitSemaphore=VK_NULL_HANDLE,VkSemaphore signalSemaphore=VK_NULL_HANDLE);
	void ShowFrame(const Frame &frame);

	/*! Create a vertex buffer and an associated device memory.
	    Want to make it a std::shared_ptr, but realized it is also shared by the GPU.
	    It is rather dangerous to go with shared_ptr.
	*/
	Buffer CreateVertexBuffer(long long int bufSize) const;

	/*! Destroys a buffer created by CreateVertexBuffer or CreateVertexBuffer.  
	    Handles stored in buf will be nullified.
	*/
	void DestroyBuffer(Buffer &buf);

	/*! Create a uniform buffer and an associated device memory.
	*/
	Buffer CreateUniformBuffer(long long int bufSize) const;

	/*! Create a 2d texture from a rgba bitmap.
	*/
	Texture2d CreateTexture2d(int wid,int hei,const unsigned char rgba[]) const;

	/*! Destroys a 2d texture.
	*/
	void DestroyTexture2d(Texture2d tex2d) const;

	/*! Allocate a uniform except for sampler (Descriptor Set in Vulkan's terminology.)
	*/
	Uniform AllocUniform(VkDescriptorSetLayout layout) const;

	/*! Set up a uniform. */
	void UpdateUniform(VkDescriptorSet descSet,Buffer uni,uint32_t binding);

	/*! Allocate a uniform for sampler (Descriptor Set in Vulkan's terminology.)
	*/
	Sampler AllocSampler(VkDescriptorSetLayout layout) const;

	/*! Set up sampler uniform for binding. */
	void UpdateSampler(VkDescriptorSet descSet,long long int nTex,const Texture2d tex2d[],uint32_t binding);

	/*! Free a uniform or a sampler.  It nullifies the descSet member. */
	void FreeUniformOrSampler(DescriptorSetAndPool &setAndPool);


	/*! Transfer data to device memory.
	*/
	void TransferToDeviceMemory(VkDeviceMemory mem,long long int len,const void *dat) const;

private:
	VkDeviceMemory AllocMemoryForBuffer(VkBuffer buf,const uint32_t memPropBit) const;
	VkDeviceMemory AllocMemoryForImage(VkImage img,const uint32_t memPropBit) const;
	VkDeviceMemory AllocMemoryForMemoryRequirement(const VkMemoryRequirements &memReq,const uint32_t memPropBit) const;

	uint32_t FindMatchingMemoryType(
	    const VkMemoryRequirements &memReq,
	    const VkPhysicalDeviceMemoryProperties &memProp,
	    const uint32_t memPropBit) const;

	void TransitionImageLayout(
	    VkCommandBuffer cmdBuf,
	    VkImage img,
	    VkImageLayout prvLayout,const uint32_t prvAccess,const uint32_t prvStage,
	    VkImageLayout newLayout,const uint32_t newAccess,const uint32_t newStage,
	    uint32_t aspectFlag) const;

	VkCommandBuffer BeginOneTimeCommandBuffer(void) const;
	void EndOneTimeCommandBuffer(VkCommandBuffer cmdBuf) const;
};

FsVulkanContext &FsVulkanGetContext(void);

std::vector <std::string> FsVulkanEnumerateInstanceExtension(void);
std::vector <const char *> FsVulkanSelectInstanceExtension(const std::vector <std::string> &foundExt);

std::vector <std::string> FsVulkanEnumerateValidationLayer(void);
std::vector <const char *> FsVulkanSelectValidationLayer(const std::vector <std::string> &foundLayer);

std::vector <VkPhysicalDevice> FsVulkanEnumeratePhysicalDevice(VkInstance inst);
VkPhysicalDevice FsVulkanSelectPhysicalDevice(const std::vector <VkPhysicalDevice> &allPhysDev,VkSurfaceKHR surface);

std::vector <VkQueueFamilyProperties> FsVulkanEnumerateQueueFamily(VkPhysicalDevice dev);

std::vector <std::string> FsVulkanEnumerateDeviceExtension(VkPhysicalDevice dev);
std::vector <const char *> FsVulkanSelectDeviceExtension(const std::vector <std::string> &foundExt);

uint32_t FsVulkanGetGraphicsQueueFamilyIndex(VkPhysicalDevice dev);
uint32_t FsVulkanGetPresentQueueFamilyIndex(VkPhysicalDevice dev,VkSurfaceKHR surface);
bool FsVulkanGetQueueFamilyIndex(uint32_t &graphicsQueueIndex,uint32_t &presentQueueIndex,VkPhysicalDevice dev,VkSurfaceKHR surface);

std::vector <VkSurfaceFormatKHR> FsVulkanEnumeratePhysicalDeviceSurfaceFormat(VkPhysicalDevice dev,VkSurfaceKHR surface);
VkSurfaceFormatKHR FsVulkanSelectSurfaceFormat(const std::vector <VkSurfaceFormatKHR> &allFormat);

std::vector <VkPresentModeKHR> FsVulkanEnumeratePhysicalDeviceSurfacePresentationModesKHR(VkPhysicalDevice dev,VkSurfaceKHR surface);
VkPresentModeKHR FsVulkanSelectPresentMode(const std::vector <VkPresentModeKHR> &allMode);

VkExtent2D FsVulkanSelectSwapExtent(const VkSurfaceCapabilitiesKHR &surfaceCap);

void FsVulkanFindDepthStencilBufferFormat(VkFormat &fom,bool &hasStencil,VkPhysicalDevice dev);

#endif



#include "fssimplewindow.h"



#ifdef __cplusplus
extern "C" {
#endif

/*! Creates a Vulkan instance.  If it fails, it's the end of the world.  What's the point to return an error code?
    If useValidationLayer is non-zero, it attaches the validation layer.
*/
VkInstance FsVulkanCreateInstance(int useValidationLayer);

/*! Transfers a platform-specific surface to FsVulkanContext.
*/
void FsVulkanSetPlatformSpecificSurface(VkSurfaceKHR surface);

/*! Selects a Vulkan Physical Device.
*/
void FsVulkanSelectPhysicalDevice(void);

/*! Create a logical device.
*/
void FsVulkanCreateLogicalDevice(void);

/*! Create a swap chain.
*/
void FsVulkanCreateSwapChain(void);

/*! Create a render pass.
*/
void FsVulkanCreateRenderPass(void);

/*! Create depth buffers. */
void FsVulkanCreateDepthBuffer(void);

/*! Create framebuffers.
*/
void FsVulkanCreateFramebuffer(void);

/*! Create command pool and command buffer.
*/
void FsVulkanCreateCommandBuffer(void);

/*! Create semaphores.
*/
void FsVulkanCreateSemaphore(void);

/*! Create stock resources. */
void FsVulkanCreateStockResource(void);

/*! Destroy a swap chain and the platform-specific surface 
    before re-creating an updated surface.
*/
void FsVulkanDestroySwapChain(void);

/*! Re-create a swap chain from a updated surface.
*/
void FsVulkanRecreateSwapChain(VkSurfaceKHR surface);


/*! Returns the surface-extension name for platform.
    Written in platform-specific source.
*/
const char *FsVulkanGetPlatformSurfacExtensionName(void);



#ifdef __cplusplus
} // extern "C"
#endif



/* } */
#endif
