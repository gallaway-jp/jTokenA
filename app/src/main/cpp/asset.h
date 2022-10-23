#ifndef JTOKENA_ASSET_H
#define JTOKENA_ASSET_H

#include <string>

std::string getAsset(const char *assetFile, void *env, void *jAssetManager);
int64_t getAsset2(const char *assetFile, void *env, void *jAssetManager, char **ppMmap);

#endif //JTOKENA_ASSET_H
