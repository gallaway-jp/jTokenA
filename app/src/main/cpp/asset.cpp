#include "asset.h"
#include "mmap.h"
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

std::string getAsset_(const char *assetFile, JNIEnv *env, jobject jAssetManager)
{
    std::string result;
    AAssetManager *am = AAssetManager_fromJava(env, jAssetManager);
    if (!am){
        return result;
    }
    AAsset *asset = AAssetManager_open(am, assetFile, AASSET_MODE_BUFFER);
    if(!asset){
        return result;
    }
    int64_t length = AAsset_getLength64(asset);
    const char *buffer = (const char *) AAsset_getBuffer(asset);
    if (!buffer) {
        return result;
    }
    result.resize(length);
    std::memcpy(&result[0], buffer, length);
    AAsset_close(asset);

    return result;
}

std::string getAsset(const char *assetFile, void *env, void *jAssetManager){
    return getAsset_(assetFile, (JNIEnv *)env, (jobject)jAssetManager);
}

#define MAX_ASSET_BUFFER_SIZE (128*1024*1024) //128MB max size

int64_t getAsset2_(const char *assetFile, JNIEnv *env, jobject jAssetManager, char **ppMmap) {
    if(!ppMmap){
        return -1;
    }

    AAssetManager *am = AAssetManager_fromJava(env, jAssetManager);
    if (!am){
        return -1;
    }
    AAsset *asset = AAssetManager_open(am, assetFile, AASSET_MODE_BUFFER);
    if(!asset){
        return -1;
    }
    int64_t length = AAsset_getLength64(asset);

    void *p =
            (::mmap(0, length, PROT_READ|PROT_WRITE, MAP_ANON|MAP_SHARED, -1, 0));
    if(p == MAP_FAILED){
        return -1;
    }

    if(length <= MAX_ASSET_BUFFER_SIZE) {
        const char *buffer = (const char *) AAsset_getBuffer(asset);
        if (!buffer) {
            return -1;
        }
        std::memcpy(p, buffer, length);
    }
    else{
        char *temp = (char *)p;
        while(true) {
            int bytes_read = AAsset_read(asset, (void *)temp, MAX_ASSET_BUFFER_SIZE);
            if (bytes_read == 0) {
                break;
            }
            temp += bytes_read;
        }
    }
    AAsset_close(asset);

    *ppMmap = (char *)p;
    return length;
}


int64_t getAsset2(const char *assetFile, void *env, void *jAssetManager, char **ppMmap){
    return getAsset2_(assetFile, (JNIEnv *)env, (jobject)jAssetManager, ppMmap);
}