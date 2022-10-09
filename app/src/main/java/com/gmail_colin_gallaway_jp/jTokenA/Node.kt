package com.gmail_colin_gallaway_jp.jTokenA

open class Features

data class Unidic29Features(val pos1: String = "",
                    val pos2: String = "",
                    val pos3: String = "",
                    val pos4: String = "",
                    val cType: String = "",
                    val cForm: String = "",
                    val lForm: String = "",
                    val lemma: String = "",
                    val orth: String = "",
                    val pron: String = "",
                    val orthBase: String = "",
                    val pronBase: String = "",
                    val goshu: String = "",
                    val iType: String = "",
                    val iForm: String = "",
                    val fType: String = "",
                    val fForm: String = "",
                    val iConType: String = "",
                    val fConType: String = "",
                    val type: String = "",
                    val kana: String = "",
                    val kanaBase: String = "",
                    val form: String = "",
                    val formBase: String = "",
                    val aType: String = "",
                    val aConType: String = "",
                    val aModType: String = "",
                    val lid: String = "",
                    val lemma_id: String = "") : Features()

data class Node(val surface: String = "",
                val features: Features = Features(),
                val length: Int = 0)
