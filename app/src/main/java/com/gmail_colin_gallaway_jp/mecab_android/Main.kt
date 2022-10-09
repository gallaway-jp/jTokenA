package com.gmail_colin_gallaway_jp.mecab_android

import java.lang.RuntimeException
import java.lang.System.loadLibrary

/**
 * A native method that is implemented by the 'mecab_android' native library,
 * which is packaged with this application.
 */
external fun tokenizeText(text: String, dicDir: String, featuresClassName: String, featuresCount: Int): String
external fun tokenizeTextAsNodes(text: String, dicDir: String, featuresClassName: String, featuresCount: Int): Object

var isMecabLoaded = false
fun loadMecab(): Boolean{
    if(!isMecabLoaded) {
        try {
            loadLibrary("mecab_android")
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