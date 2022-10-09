#include "native-lib.h"
#include "mecab.h"

#include <cstring>
#include <sstream>
#include <iostream>
#include <vector>
#include <fstream>

Tagger* jCreateTagger(JNIEnv *env, jstring jDicDir)
{
    std::string arg = "";

    const char *nativeDicdir = env->GetStringUTFChars(jDicDir, NULL);
    std::string dicdir = nativeDicdir;
    env->ReleaseStringUTFChars(jDicDir, nativeDicdir);

    std::string mecabrc = dicdir + "/" + "mecabrc";

    std::string system = "mecab-android";
    arg = system + " -r " + mecabrc + " -d " + dicdir + arg;

    std::vector<std::string> strings;
    std::istringstream f(arg);
    std::string s;
    while (getline(f, s, ' ')) {
        std::cout << s << std::endl;
        strings.push_back(s);
    }
    int argc = strings.size();
    char** argv = (char**)malloc(argc * sizeof(char*));

    if(!argv){
        return NULL;
    }

    for(int i = 0; i<argc; i++){
        const char *cstr = (strings.at(i).c_str());
        argv[i] = (char *)cstr;
    }

    Tagger *tagger = createTagger(argc, argv);

    free(argv);
    return tagger;
}

typedef struct _CLASSINFO{
    jclass featuresClass;
    jmethodID featuresID;
    jclass nodeClass;
    jmethodID nodeID;
} CLASSINFO;

std::vector<jobject> createJNodesVector(JNIEnv *env, Node *nodes, CLASSINFO classInfo)
{
    std::vector<jobject> vector;
    Node *temp = nodes;
    do{
        if(temp->length > 0) {
            std::string surface = temp->surface;
            surface = surface.substr(0, temp->length);
            jstring jSurface = env->NewStringUTF(surface.c_str());

            std::vector<jvalue> properties;
            std::string feature = "";
            std::string features = temp->feature;
            for(size_t i = 0; i < features.length(); i++){
                if (features[i] == ','){
                    jvalue jVal;
                    jVal.l = env->NewStringUTF(feature.c_str());
                    if(jVal.l != NULL) {
                        properties.push_back(jVal);
                    }
                    feature = "";
                    continue;
                }
                else if (features[i] == '\"'){
                    i++;
                    size_t end = features.find('\"', i);
                    if(end == std::string::npos) {
                        feature.append(features, i);
                        break;
                    }
                    size_t length = end-i;
                    if(length == 0){
                        continue;
                    }
                    feature.append(features, i, length);
                    i += length-1;
                    continue;
                }
                feature.append(1, features[i]);
            }
            {
                jvalue jVal;
                jVal.l = env->NewStringUTF(feature.c_str());
                if(jVal.l != NULL) {
                    properties.push_back(jVal);
                }
            }

            jobject jFeatures = env->NewObjectA(classInfo.featuresClass, classInfo.featuresID,
                                                (jvalue *) &properties[0]);
            vector.push_back(env->NewObject(classInfo.nodeClass, classInfo.nodeID, jSurface, jFeatures, temp->length));
        }
        temp = temp->next;
    }
    while(temp);
    return vector;
}

CLASSINFO getClassInfo(JNIEnv *env, jstring jfeaturesClassName, int featuresCount)
{
    jclass nodeClass = env->FindClass("com/gmail_colin_gallaway_jp/jTokenA/Node");
    if(!nodeClass){
        return {0};
    }
    jmethodID nodeID = env->GetMethodID(nodeClass, "<init>",
                                        "(Ljava/lang/String;Lcom/gmail_colin_gallaway_jp/jTokenA/Features;I)V");
    if(!nodeID){
        return {0};
    }

    const char *nativeFeaturesClass = env->GetStringUTFChars(jfeaturesClassName, NULL);
    std::string featuresClassName = nativeFeaturesClass;
    env->ReleaseStringUTFChars(jfeaturesClassName, nativeFeaturesClass);
    jclass featuresClass = env->FindClass(featuresClassName.c_str());
    if(!featuresClass){
        return {0};
    }

    std::string sig = "(";
    for(int i = 0; i < featuresCount; i++){
        sig.append("Ljava/lang/String;");
    }
    sig.append(")V");

    jmethodID featuresID = env->GetMethodID(featuresClass, "<init>", sig.c_str());
    if(!featuresID){
        return {0};
    }
    return {featuresClass, featuresID, nodeClass, nodeID};
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_gmail_1colin_1gallaway_1jp_jTokenA_MainKt_tokenizeText(JNIEnv *env, jobject thiz,
                                                                        jstring text,
                                                                        jstring dic_dir,
                                                                        jstring features_class_name,
                                                                        jint features_count) {
    // "/storage/emulated/0/Download/unidic-cwj-3.1.0-full"
    // "/mnt/user/0/primary/Download/unidic-cwj-3.1.0-full"

    Tagger *tagger = jCreateTagger(env, dic_dir);
    if (!tagger) {
        return NULL;
    }

    const char *nativeInput = env->GetStringUTFChars(text, NULL);
    std::string input = nativeInput;
    env->ReleaseStringUTFChars(text, nativeInput);

    const char *result = tagger->parse(input.c_str(), input.length());

    deleteTagger(tagger);

    if(result){
        return env->NewStringUTF(result);
    }
    return NULL;
}
extern "C"
JNIEXPORT jobject JNICALL
Java_com_gmail_1colin_1gallaway_1jp_jTokenA_MainKt_tokenizeTextAsNodes(JNIEnv *env,
                                                                               jobject thiz,
                                                                               jstring text,
                                                                               jstring dic_dir,
                                                                               jstring features_class_name,
                                                                               jint features_count) {
    Tagger *tagger = jCreateTagger(env, dic_dir);
    if (!tagger) {
        return NULL;
    }

    const char *nativeInput = env->GetStringUTFChars(text, NULL);
    std::string input = nativeInput;
    env->ReleaseStringUTFChars(text, nativeInput);

    const Node *result = tagger->parseToNode(input.c_str());
    if(!result){
        return NULL;
    }

    CLASSINFO classInfo = getClassInfo(env, features_class_name, (int)features_count);
    std::vector<jobject> nodes = createJNodesVector(env, (Node*)result, classInfo);

    deleteTagger(tagger);

    jobjectArray jArray = env->NewObjectArray(nodes.size(), classInfo.nodeClass , NULL);
    if(!jArray){
        return NULL;
    }
    for (size_t aI = 0; aI < nodes.size(); aI++) {
        env->SetObjectArrayElement(jArray, aI, nodes[aI]);
    }

    return jArray;
}