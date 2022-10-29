package com.gmail_colin_gallaway_jp.jTokenA

import android.content.res.AssetManager

private external fun createTagger(dicDir: String): Long
private external fun createTagger2(assetsFolder: String, assetManager: AssetManager): Long
private external fun tokenizeTextAsString(tagger: Long, text: String): String
private external fun tokenizeTextAsNodes(tagger: Long, text: String, featuresClassName: String, featuresCount: Int): Array<Node>?
private external fun deleteTagger(tagger: Long)

class Tagger {
    private val featuresCount : Int
    private val className: String
    private var tagger: Long = 0
    constructor(
        dicDir: String,
        features: Features = Unidic29Features()
    ): this(features) {
        tagger = createTagger(dicDir)
    }
    constructor(
        assetsFolder: String, assetManager: AssetManager,
        features: Features = Unidic29Features(),
    ): this(features) {
        tagger = createTagger2(assetsFolder, assetManager)
    }
    private constructor(features: Features){
        featuresCount = features::class.java.declaredFields.size
        className = features::class.java.name.replace('.','/')
    }
    init {
        System.loadLibrary("jTokenA")
    }

    fun tokenizeAsString(input: String): String
    {
        if(tagger == 0L){
            return ""
        }
        return tokenizeTextAsString(tagger, input)
    }

    fun tokenizeAsNodes(input: String): Array<Node>? {
        if (tagger == 0L) {
            return null
        }
        return tokenizeTextAsNodes(tagger, input, className, featuresCount)
    }

    fun deleteTagger(){
        if(tagger == 0L){
            return
        }
        deleteTagger(tagger)
        tagger = 0
    }
    fun finalize(){
        deleteTagger()
    }
}