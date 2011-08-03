/*
 * LTE Game Engine
 * Copyright (C) 2006-2008 SiberianSTAR <haxormail@gmail.com>
 * http://www.ltestudios.com
 *  
 * The LTE Game Engine is based on Irrlicht 1.0
 * Irrlicht Engine is Copyright (C) 2002-2006 Nikolaus Gebhardt
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA, or see the FSF site: http://www.fsf.org.
 */
 
#ifndef __C_VIDEO_NULL_H_INCLUDED__
#define __C_VIDEO_NULL_H_INCLUDED__

#include "IVideoDriver.h"
#include "IFileSystem.h"
#include "IImagePresenter.h"
#include "engineArray.h"
#include "engineString.h"
#include "IImageLoader.h"
#include "ISceneManager.h"
#include "CFPSCounter.h"
#include "S3DVertex.h"

namespace engine
{
namespace video
{
class CNullDriver : public IVideoDriver, public IGPUProgrammingServices
{
public:

//! constructor
CNullDriver(io::IFileSystem* io, const core::dimension2d<s32>& screenSize);

//! destructor
virtual ~CNullDriver();

virtual bool beginScene(bool backBuffer, bool zBuffer, SColor color);

virtual bool endScene( s32 windowId = 0, core::rect<s32>* sourceRect=0 );

//! queries the features of the driver, returns true if feature is available
virtual bool queryFeature(E_VIDEO_DRIVER_FEATURE feature);

//! sets transformation
virtual void setTransform(E_TRANSFORMATION_STATE state, const core::matrix4& mat);
virtual void setSceneManager(void *smgr);

//! sets a material
virtual void setMaterial(const SMaterial& material);

//! loads a Texture
virtual ITexture* getTexture(const c8* filename);

//! loads a Texture
virtual ITexture* getTexture(io::IReadFile* file);

virtual void enableClippingFeature(bool enable);

//! creates a Texture
virtual ITexture* addTexture(const core::dimension2d<s32>& size, const c8* name, ECOLOR_FORMAT format);

//! sets a render target
virtual bool setRenderTarget(video::ITexture* texture, bool clearBackBuffer,
                             bool clearZBuffer, SColor color);

//! sets a viewport
virtual void setViewPort(const core::rect<s32>& area);

//! gets the area of the current viewport
virtual const core::rect<s32>& getViewPort() const;

//! draws an indexed triangle list
virtual void drawIndexedTriangleList(const S3DVertex* vertices, s32 vertexCount, const u16* indexList, s32 triangleCount);

//! draws an indexed triangle list
virtual void drawIndexedTriangleList(const S3DVertex2TCoords* vertices, s32 vertexCount, const u16* indexList, s32 triangleCount);

//! Draws an indexed triangle list.
virtual void drawIndexedTriangleList(const S3DVertexTangents* vertices,
                                     s32 vertexCount, const u16* indexList, s32 triangleCount);

//! Draws an indexed triangle fan.
virtual void drawIndexedTriangleFan(const S3DVertex* vertices,
                                    s32 vertexCount, const u16* indexList, s32 triangleCount);

//! Draws an indexed triangle list.
virtual void drawIndexedTriangleFan(const S3DVertex2TCoords* vertices,
                                    s32 vertexCount, const u16* indexList, s32 triangleCount);

//! Draws a 3d line.
virtual void draw3DLine(const core::vector3df& start,
                        const core::vector3df& end, SColor color = SColor(255,255,255,255));

//! Draws a 3d triangle.
virtual void draw3DTriangle(const core::triangle3df& triangle,
                            SColor color = SColor(255,255,255,255));

//! Draws a 3d axis aligned box.
virtual void draw3DBox(const core::aabbox3d<f32> box,
                       SColor color = SColor(255,255,255,255));

//! draws an 2d image
virtual void draw2DImage(video::ITexture* texture, const core::position2d<s32>& destPos);

virtual void changeCursor(video::ITexture *texture);

//! draws an 2d image, using a color (if color is other then Color(255,255,255,255)) and the alpha channel of the texture if wanted.
virtual void draw2DImage(video::ITexture* texture, const core::position2d<s32>&destPos,
                         const core::rect<s32>&sourceRect, const core::rect<s32>* clipRect = 0,
                         SColor color=SColor(255,255,255,255), bool useAlphaChannelOfTexture=false);

virtual void draw2DStretchedImage(video::ITexture* texture, const core::position2d<s32>&pos,
                                  const core::dimension2d<s32>&size, core::rect<s32>&sourceRect, SColor color = SColor(255,255,255,255),
                                  bool useAlphaChannelOfTexture = false) ;

//! Draws a part of the texture into the rectangle.
virtual void draw2DImage(video::ITexture* texture, const core::rect<s32>& destRect,
                         const core::rect<s32>& sourceRect, const core::rect<s32>* clipRect = 0,
                         video::SColor* colors=0, bool useAlphaChannelOfTexture=false);

//! draw an 2d rectangle
virtual void draw2DRectangle(SColor color, const core::rect<s32>& pos, const core::rect<s32>* clip = 0);

//!Draws an 2d rectangle with a gradient.
virtual void draw2DRectangle(const core::rect<s32>& pos,
                             SColor colorLeftUp, SColor colorRightUp, SColor colorLeftDown, SColor colorRightDown,
                             const core::rect<s32>* clip = 0);

//! Draws a 2d line.
virtual void draw2DLine(const core::position2d<s32>&start,
                        const core::position2d<s32>&end,
                        SColor color=SColor(255,255,255,255));

//! Draws a non filled concyclic reqular 2d polyon.
virtual void draw2DPolygon(core::position2d<s32> center,
                           f32 radius, video::SColor Color, s32 vertexCount);

virtual void setFog(SColor color=SColor(0,255,255,255), bool linearFog=true,
                    f32 start=50.0f, f32 end=100.0f,
                    f32 density=0.01f, bool pixelFog=false, bool rangeFog=false);

//! returns screen size
virtual core::dimension2d<s32> getScreenSize();

// returns current frames per second value
virtual s32 getFPS();

//! returns amount of primitives (mostly triangles) were drawn in the last frame.
//! very useful method for statistics.
virtual u32 getPrimitiveCountDrawn();

//! deletes all dynamic lights there are
virtual void deleteAllDynamicLights();

//! adds a dynamic light
virtual void addDynamicLight(const SLight& light);

//! returns the maximal amount of dynamic lights the device can handle
virtual s32 getMaximalDynamicLightAmount();

//! \return Returns the name of the video driver. Example: In case of the DIRECT3D8
//! driver, it would return "Direct3D8.1".
virtual const wchar_t* getName();

//! Sets the dynamic ambient light color. The default color is
//! (0,0,0,0) which means it is dark.
//! \param color: New color of the ambient light.
virtual void setAmbientLight(const SColorf& color);

//! Adds an external image loader to the engine.
virtual void addExternalImageLoader(IImageLoader* loader);

//! Draws a shadow volume into the stencil buffer. To draw a stencil shadow, do
//! this: Frist, draw all geometry. Then use this method, to draw the shadow
//! volume. Then, use IVideoDriver::drawStencilShadow() to visualize the shadow.
virtual void drawStencilShadowVolume(const core::vector3df* triangles, s32 count, bool zfail=true);

//! Fills the stencil shadow with color. After the shadow volume has been drawn
//! into the stencil buffer using IVideoDriver::drawStencilShadowVolume(), use this
//! to draw the color of the shadow.
virtual void drawStencilShadow(bool clearStencilBuffer=false,
                               video::SColor leftUpEdge = video::SColor(0,0,0,0),
                               video::SColor rightUpEdge = video::SColor(0,0,0,0),
                               video::SColor leftDownEdge = video::SColor(0,0,0,0),
                               video::SColor rightDownEdge = video::SColor(0,0,0,0));

//! Returns current amount of dynamic lights set
//! \return Current amount of dynamic lights set
virtual s32 getDynamicLightCount();

//! Returns light data which was previously set with IVideDriver::addDynamicLight().
//! \param idx: Zero based index of the light. Must be greater than 0 and smaller
//! than IVideoDriver()::getDynamicLightCount.
//! \return Light data.
virtual const SLight& getDynamicLight(s32 idx);

//! Removes a texture from the texture cache and deletes it, freeing lot of
//! memory.
virtual void removeTexture(ITexture* texture);

//! Removes all texture from the texture cache and deletes them, freeing lot of
//! memory.
virtual void removeAllTextures();

//! Returns a pointer to a texture
/** Returns the pointer of the texture with the index specified wich
   is stored into the texture cache.
 \param index: Index of the texture
 */
virtual ITexture *getTextureByIndex(u32 Index);

//! Returns the count of the texture in cache
virtual u32 getTextureCount();

//! Creates a render target texture.
virtual ITexture* createRenderTargetTexture(core::dimension2d<s32> size);

//! Creates an 1bit alpha channel of the texture based of an color key.
virtual void makeColorKeyTexture(video::ITexture* texture, video::SColor color);

//! Creates an 1bit alpha channel of the texture based of an color key position.
virtual void makeColorKeyTexture(video::ITexture* texture, core::position2d<s32> colorKeyPixelPos);

//! Creates a normal map from a height map texture.
//! \param amplitude: Constant value by which the height information is multiplied.
virtual void makeNormalMapTexture(video::ITexture* texture, f32 amplitude=1.0f);

//! Returns the maximum amount of primitives (mostly vertices) which
//! the device is able to render with one drawIndexedTriangleList
//! call.
virtual s32 getMaximalPrimitiveCount();

//! Enables or disables a texture creation flag.
virtual void setTextureCreationFlag(E_TEXTURE_CREATION_FLAG flag, bool enabled);

//! Returns if a texture creation flag is enabled or disabled.
virtual bool getTextureCreationFlag(E_TEXTURE_CREATION_FLAG flag);

//! Creates a software image from a file.
virtual IImage* createImageFromFile(const char* filename);

//! Creates a software image from a file.
virtual IImage* createImageFromFile(io::IReadFile* file);

//! Creates a software image from a byte array.
//! \param useForeignMemory: If true, the image will use the data pointer
//! directly and own it from now on, which means it will also try to delete [] the
//! data when the image will be destructed. If false, the memory will by copied.
virtual IImage* createImageFromData(ECOLOR_FORMAT format,
                                    const core::dimension2d<s32>& size, void *data, bool ownForeignMemory=true);

//! Draws a mesh buffer
virtual void drawMeshBuffer(scene::IMeshBuffer* mb);

//! Only used by the internal engine. Used to notify the driver that
//! the window was resized.
virtual void OnResize(const core::dimension2d<s32>& size);

//! Adds a new material renderer to the video device.
virtual s32 addMaterialRenderer(IMaterialRenderer* renderer);

//! Returns driver and operating system specific data about the IVideoDriver.
virtual SExposedVideoData getExposedVideoData();

//! Returns type of video driver
virtual E_DRIVER_TYPE getDriverType();

//! Returns the transformation set by setTransform
virtual core::matrix4 getTransform(E_TRANSFORMATION_STATE state);

//! Returns pointer to the IGPUProgrammingServices interface.
virtual IGPUProgrammingServices* getGPUProgrammingServices();

//! Adds a new material renderer to the VideoDriver, using pixel and/or
//! vertex shaders to render geometry.
virtual s32 addShaderMaterial(const c8* vertexShaderProgram = 0,
                              const c8* pixelShaderProgram = 0,
                              IShaderConstantSetCallBack* callback = 0,
                              E_MATERIAL_TYPE baseMaterial = video::EMT_SOLID,
                              s32 userData=0);

//! Like IGPUProgrammingServices::addShaderMaterial(), but tries to load the
//! programs from files.
virtual s32 addShaderMaterialFromFiles(io::IReadFile* vertexShaderProgram = 0,
                                       io::IReadFile* pixelShaderProgram = 0,
                                       IShaderConstantSetCallBack* callback = 0,
                                       E_MATERIAL_TYPE baseMaterial = video::EMT_SOLID,
                                       s32 userData=0);

//! Like IGPUProgrammingServices::addShaderMaterial(), but tries to load the
//! programs from files.
virtual s32 addShaderMaterialFromFiles(const c8* vertexShaderProgramFileName = 0,
                                       const c8* pixelShaderProgramFileName = 0,
                                       IShaderConstantSetCallBack* callback = 0,
                                       E_MATERIAL_TYPE baseMaterial = video::EMT_SOLID,
                                       s32 userData=0);

//! Returns pointer to material renderer or null
virtual IMaterialRenderer* getMaterialRenderer(s32 idx);

//! Returns amount of currently available material renderers.
virtual s32 getMaterialRendererCount();

//! Adds a new material renderer to the VideoDriver, based on a high level shading
//! language. Currently only HLSL in D3D9 is supported.
virtual s32 addHighLevelShaderMaterial(
        const c8* vertexShaderProgram,
        const c8* vertexShaderEntryPointName = 0,
        E_VERTEX_SHADER_TYPE vsCompileTarget = EVST_VS_1_1,
        const c8* pixelShaderProgram = 0,
        const c8* pixelShaderEntryPointName = 0,
        E_PIXEL_SHADER_TYPE psCompileTarget = EPST_PS_1_1,
        IShaderConstantSetCallBack* callback = 0,
        E_MATERIAL_TYPE baseMaterial = video::EMT_SOLID,
        s32 userData=0);

//! Like IGPUProgrammingServices::addShaderMaterial() (look there for a detailed description),
//! but tries to load the programs from files.
virtual s32 addHighLevelShaderMaterialFromFiles(
        const c8* vertexShaderProgram,
        const c8* vertexShaderEntryPointName = "main",
        E_VERTEX_SHADER_TYPE vsCompileTarget = EVST_VS_1_1,
        const c8* pixelShaderProgram = 0,
        const c8* pixelShaderEntryPointName = "main",
        E_PIXEL_SHADER_TYPE psCompileTarget = EPST_PS_1_1,
        IShaderConstantSetCallBack* callback = 0,
        E_MATERIAL_TYPE baseMaterial = video::EMT_SOLID,
        s32 userData=0);

//! Like IGPUProgrammingServices::addShaderMaterial() (look there for a detailed description),
//! but tries to load the programs from files.
virtual s32 addHighLevelShaderMaterialFromFiles(
        io::IReadFile* vertexShaderProgram,
        const c8* vertexShaderEntryPointName = "main",
        E_VERTEX_SHADER_TYPE vsCompileTarget = EVST_VS_1_1,
        io::IReadFile* pixelShaderProgram = 0,
        const c8* pixelShaderEntryPointName = "main",
        E_PIXEL_SHADER_TYPE psCompileTarget = EPST_PS_1_1,
        IShaderConstantSetCallBack* callback = 0,
        E_MATERIAL_TYPE baseMaterial = video::EMT_SOLID,
        s32 userData=0);

//! Clears the ZBuffer.
virtual void clearZBuffer();

protected:
bool enableClipping;

//! deletes all textures
void deleteAllTextures();

//! looks if the image is already loaded
video::ITexture* findTexture(const c8* filename);

//! opens the file and loads it into the surface
video::ITexture* loadTextureFromFile(io::IReadFile* file);

//! adds a surface, not loaded or created by the engine Engine
void addTexture(video::ITexture* surface, const c8* filename);

//! Creates a texture from a loaded IImage.
virtual ITexture* addTexture(const c8* name, IImage* image);

//! returns a device dependent texture from a software surface (IImage)
//! THIS METHOD HAS TO BE OVERRIDDEN BY DERIVED DRIVERS WITH OWN TEXTURES
virtual video::ITexture* createDeviceDependentTexture(IImage* surface);

//! checks triangle count and print warning if wrong
bool checkPrimitiveCount(s32 prmcnt);

// adds a material renderer and drops it afterwards. To be used for internal creation
s32 addAndDropMaterialRenderer(IMaterialRenderer* m);

//! deletes all material renderers
void deleteMaterialRenders();

// prints renderer version
void printVersion();

//! normal map loopup 32 bit version
inline f32 nml32(int x, int y, int pitch, int height, s32 *p)
{
	if (x < 0) x = pitch-1;if (x >= pitch) x = 0;
	if (y < 0) y = height-1;if (y >= height) y = 0;
	return (f32)(((p[(y * pitch) + x])>>16) & 0xff);
}

//! normal map loopup 16 bit version
inline f32 nml16(int x, int y, int pitch, int height, s16 *p)
{
	if (x < 0) x = pitch-1;if (x >= pitch) x = 0;
	if (y < 0) y = height-1;if (y >= height) y = 0;
	return (f32) video::getLuminance(p[(y * pitch) + x]);
}



struct SSurface
{
	core::stringc Filename;
	video::ITexture* Surface;

	bool operator < (const SSurface& other) const
	{
		return (Filename < other.Filename);
	}
};

core::array<SSurface> Textures;
core::array<video::IImageLoader*> SurfaceLoader;
core::array<SLight> Lights;
core::array<IMaterialRenderer*> MaterialRenderers;

io::IFileSystem* FileSystem;

core::rect<s32> ViewPort;
core::dimension2d<s32> ScreenSize;

CFPSCounter FPSCounter;

u32 PrimitivesDrawn;

u32 TextureCreationFlags;

bool LinearFog;
f32 FogStart;
f32 FogEnd;
f32 FogDensity;
bool PixelFog;
bool RangeFog;
SColor FogColor;

SExposedVideoData ExposedData;
};

} // end namespace video
} // end namespace engine


#endif

