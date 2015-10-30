//
//  BufferImpl.m
//  MetalCube
//
//  Created by William Lindmeier on 10/17/15.
//
//

#import "BufferImpl.h"
#include "Buffer.h"
#import <QuartzCore/CAMetalLayer.h>
#import <Metal/Metal.h>
#import <simd/simd.h>
#import "metal.h"
#import "RendererMetalImpl.h"

using namespace cinder;
using namespace cinder::mtl;
using namespace cinder::cocoa;

@implementation BufferImpl

- (instancetype)initWithBytes:(const void *)pointer length:(unsigned long)length label:(NSString *)label
{
    self = [super init];
    if ( self )
    {
        auto device = [RendererMetalImpl sharedRenderer].device;
        
        // TODO: Let the user pass in options
        if ( pointer != NULL )
        {
            self.buffer = [device newBufferWithBytes:pointer
                                              length:length
                                             options:MTLResourceCPUCacheModeDefaultCache];
        }
        else
        {
            self.buffer = [device newBufferWithLength:length options:0];
            
        }
        
        self.buffer.label = label;
    }
    return self;
}

@end