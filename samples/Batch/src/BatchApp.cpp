#include "cinder/app/App.h"
#include "metal.h"
#include "SharedTypes.h"
#include "Batch.h"

using namespace ci;
using namespace ci::app;
using namespace std;

float cubeVertexData[216] =
{
    0.5, -0.5, 0.5, 0.0, -1.0,  0.0, -0.5, -0.5, 0.5, 0.0, -1.0, 0.0, -0.5, -0.5, -0.5, 0.0, -1.0,  0.0, 0.5, -0.5, -0.5,  0.0, -1.0,  0.0, 0.5, -0.5, 0.5, 0.0, -1.0,  0.0, -0.5, -0.5, -0.5, 0.0, -1.0,  0.0, 0.5, 0.5, 0.5,  1.0, 0.0,  0.0, 0.5, -0.5, 0.5, 1.0,  0.0,  0.0, 0.5, -0.5, -0.5,  1.0,  0.0,  0.0, 0.5, 0.5, -0.5, 1.0, 0.0,  0.0, 0.5, 0.5, 0.5,  1.0, 0.0,  0.0, 0.5, -0.5, -0.5,  1.0,  0.0,  0.0, -0.5, 0.5, 0.5,  0.0, 1.0,  0.0, 0.5, 0.5, 0.5,  0.0, 1.0,  0.0, 0.5, 0.5, -0.5, 0.0, 1.0,  0.0, -0.5, 0.5, -0.5, 0.0, 1.0,  0.0, -0.5, 0.5, 0.5,  0.0, 1.0,  0.0, 0.5, 0.5, -0.5, 0.0, 1.0,  0.0, -0.5, -0.5, 0.5,  -1.0,  0.0, 0.0, -0.5, 0.5, 0.5, -1.0, 0.0,  0.0, -0.5, 0.5, -0.5,  -1.0, 0.0,  0.0, -0.5, -0.5, -0.5,  -1.0,  0.0,  0.0, -0.5, -0.5, 0.5,  -1.0,  0.0, 0.0, -0.5, 0.5, -0.5,  -1.0, 0.0,  0.0, 0.5, 0.5,  0.5,  0.0, 0.0,  1.0, -0.5, 0.5,  0.5,  0.0, 0.0,  1.0, -0.5, -0.5, 0.5, 0.0,  0.0, 1.0, -0.5, -0.5, 0.5, 0.0,  0.0, 1.0, 0.5, -0.5, 0.5, 0.0,  0.0,  1.0, 0.5, 0.5,  0.5,  0.0, 0.0,  1.0, 0.5, -0.5, -0.5,  0.0,  0.0, -1.0, -0.5, -0.5, -0.5, 0.0,  0.0, -1.0, -0.5, 0.5, -0.5,  0.0, 0.0, -1.0, 0.5, 0.5, -0.5,  0.0, 0.0, -1.0, 0.5, -0.5, -0.5,  0.0,  0.0, -1.0, -0.5, 0.5, -0.5,  0.0, 0.0, -1.0
};

class BatchApp : public App
{
public:
    
    void setup() override;
    void resize() override;
    void update() override;
    void draw() override;
    
    float mRotation;
    
    mtl::RenderPassDescriptorRef mRenderDescriptor;
    mtl::UniformBlock<mtl::ciUniforms_t> mUniformBlock;
    
    mtl::RenderPipelineStateRef mPipelineSource;
    mtl::BatchRef mBatchSource;

    mtl::RenderPipelineStateRef mPipelineVertBuffer;
    mtl::BatchRef mBatchVertBuffer;

    mtl::DepthStateRef mDepthEnabled;
    mtl::DepthStateRef mDepthDisabled;
    
    mtl::TextureBufferRef mTexture;
    
    CameraPersp mCamera;
};

void BatchApp::setup()
{
    mRenderDescriptor = mtl::RenderPassDescriptor::create(mtl::RenderPassDescriptor::Format()
                                                          .clearColor(ColorAf(0.f,1.f,1.f)));
    
    mPipelineSource = mtl::RenderPipelineState::create("lighting_vertex_interleaved_src",
                                                       "lighting_texture_fragment");
    
    mTexture = mtl::TextureBuffer::create(loadImage(getAssetPath("checker.png")),
                                          mtl::TextureBuffer::Format().mipmapLevel(4));

    mDepthEnabled = mtl::DepthState::create( mtl::DepthState::Format().depthWriteEnabled() );
    mDepthDisabled = mtl::DepthState::create( mtl::DepthState::Format()
                                              .depthWriteEnabled(false) );
    
    // Set up a couple different kinds of Batches.
    
    // Basic Geom Source
    mBatchSource = mtl::Batch::create(geom::Cube(), mPipelineSource);

    // Custom Vertex Buffer w/ non-interleaved attribute buffers
    vector<vec3> positions;
    vector<vec3> normals;
    // iterate over cube data and split into verts and normals
    for ( int i = 0; i < 36; ++i )
    {
        vec3 pos(cubeVertexData[i*6+0], cubeVertexData[i*6+1], cubeVertexData[i*6+2]);
        vec3 norm(cubeVertexData[i*6+3], cubeVertexData[i*6+4], cubeVertexData[i*6+5]);
        positions.push_back(pos);
        normals.push_back(norm);
    }
    
    mtl::VertexBufferRef attribBufferCube = mtl::VertexBuffer::create( positions.size() );
    mtl::DataBufferRef positionBuffer = mtl::DataBuffer::create(positions, mtl::DataBuffer::Format().label("Positions"));
    attribBufferCube->setBufferForAttribute(positionBuffer, ci::geom::POSITION);
    mtl::DataBufferRef normalBuffer = mtl::DataBuffer::create(normals, mtl::DataBuffer::Format().label("Normals"));
    attribBufferCube->setBufferForAttribute(normalBuffer, ci::geom::NORMAL);
    
    mPipelineVertBuffer = mtl::RenderPipelineState::create("lighting_vertex_attrib_buffers",
                                                           "lighting_fragment",
                                                           mtl::RenderPipelineState::Format());
    
    mBatchVertBuffer = mtl::Batch::create( attribBufferCube, mPipelineVertBuffer );
}

void BatchApp::resize()
{
    mCamera.setPerspective(65.f, getWindowAspectRatio(), 0.01, 1000.f);
    mCamera.lookAt(vec3(0,0,-5),vec3(0));
}

void BatchApp::update()
{
    mRotation += 0.01f;
    mat4 modelMatrix = glm::rotate( mRotation, vec3(1.0f, 1.0f, 1.0f) );
    mat4 normalMatrix = inverse(transpose(modelMatrix));
    mat4 modelViewMatrix = mCamera.getViewMatrix() * modelMatrix;
    mat4 modelViewProjectionMatrix = mCamera.getProjectionMatrix() * modelViewMatrix;

    mUniformBlock.updateData( [&]( mtl::ciUniforms_t data )
    {
        data.ciModelMatrix = toMtl(modelMatrix);
        data.ciNormalMatrix = toMtl(normalMatrix);
        data.ciProjectionMatrix = toMtl(mCamera.getProjectionMatrix());
        data.ciModelViewProjectionMatrix = toMtl(modelViewProjectionMatrix);
        return data;
    });
}

void BatchApp::draw()
{
    mtl::ScopedRenderCommandBuffer renderBuffer;
    mtl::ScopedRenderEncoder renderEncoder = renderBuffer.scopedRenderEncoder(mRenderDescriptor);
    
    renderEncoder.setDepthStencilState(mDepthEnabled);
    
    mUniformBlock.sendToEncoder(renderEncoder);
    
    renderEncoder.setTexture(mTexture);
    
    // Put your drawing here
    mBatchSource->draw(renderEncoder);
    
//    // TEST
    
// << START CONTEXT BLOCK
//    renderEncoder.setDepthStencilState(mDepthDisabled);
    
    mat4 modelMatrix = glm::translate(mat4(1), vec3(2,0,0));
    modelMatrix = glm::rotate(modelMatrix, -mRotation, vec3(1.0f, 1.0f, 1.0f));
    
    mat4 normalMatrix = inverse(transpose(modelMatrix));
    mat4 modelViewMatrix = mCamera.getViewMatrix() * modelMatrix;
    mat4 modelViewProjectionMatrix = mCamera.getProjectionMatrix() * modelViewMatrix;
    
    mUniformBlock.updateData( [&]( mtl::ciUniforms_t data )
                              {
                                 data.ciModelMatrix = toMtl(modelMatrix);
                                 data.ciNormalMatrix = toMtl(normalMatrix);
                                 data.ciModelViewProjectionMatrix = toMtl(modelViewProjectionMatrix);
                                 return data;
                              });
    mUniformBlock.sendToEncoder( renderEncoder );
    
// << END CONTEXT BLOCK

    mBatchVertBuffer->draw(renderEncoder);
}

CINDER_APP( BatchApp, RendererMetal )