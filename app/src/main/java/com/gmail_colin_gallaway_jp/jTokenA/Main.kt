package com.gmail_colin_gallaway_jp.jTokenA

import android.content.Context
import android.content.res.AssetManager
import java.lang.RuntimeException
import java.lang.System.loadLibrary

/**
 * A native method that is implemented by the 'jTokenA' native library,
 * which is packaged with this application.
 */
external fun tokenizeText(text: String, dicDir: String, featuresClassName: String, featuresCount: Int): String
external fun tokenizeTextAsNodes(text: String, dicDir: String, featuresClassName: String, featuresCount: Int): Object
external fun tokenizeTextAsNodes2(text: String, assetsFolder: String, featuresClassName: String, featuresCount: Int, assetManager: AssetManager): Object

var isMecabLoaded = false
private fun loadMecab(): Boolean{
    if(!isMecabLoaded) {
        try {
            loadLibrary("jTokenA")
        } catch (e: RuntimeException) {
            return false
        }
        isMecabLoaded = true
    }
    return true
}

fun tokenizeAsString(input: String, dicDir: String,
                     features: Features = Unidic29Features()): String
{
    if(!loadMecab()){
        return ""
    }

    val featuresCount = features::class.java.declaredFields.size
    val className = features::class.java.name.replace('.','/')

    return tokenizeText(input, dicDir, className, featuresCount)
}

fun tokenizeAsNodes(input: String, dicDir: String,
                    features: Features = Unidic29Features()): Array<Node>?
{
    if(!loadMecab()){
        return null
    }

    val featuresCount = features::class.java.declaredFields.size
    val className = features::class.java.name.replace('.','/')

    return tokenizeTextAsNodes(input, dicDir, className, featuresCount) as Array<Node>
}

fun tokenizeAsNodes2(input: String, assetsFolder: String, assetManager: AssetManager,
                    features: Features = Unidic29Features()): Array<Node>?
{
    if(!loadMecab()){
        return null
    }

    val featuresCount = features::class.java.declaredFields.size
    val className = features::class.java.name.replace('.','/')

    return tokenizeTextAsNodes2(input, assetsFolder, className, featuresCount, assetManager) as Array<Node>
}