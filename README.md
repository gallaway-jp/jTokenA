# mecab_android
A MeCab (Japanese tokenizer) wrapper for the Android operating system written in native c++ and Kotlin

## Installation

This project is an Android library, so you will need to build the Android Archive (AAR) file.
Once built, add the AAR file to your project.
A Mecab compatible dictionary will also be required.
The [Unidic](https://clrd.ninjal.ac.jp/unidic/en/) dictionary is one such dictionary.
If using a dictionary other than Unidic version 3.1.0, 
a data class of all the features (as Strings) in the dictionary will need to be defined
and passed to either the tokenizeAsString or tokenizeAsNodes functions.

## Usage (Kotlin)
```kotlin
import com.gmail_colin_gallaway_jp.mecab_android.tokenizeAsNodes
val input = "太郎は次郎が持っている本を花子に渡した。"
// Storage read permission will be required
val dicDir = "/storage/emulated/0/Download/unidic-cwj-3.1.0-full"
val result = tokenizeAsNodes(input, dicDir)
```


## Other wrappers

There are alternative MeCab wrappers, such as [fugashi](https://github.com/polm/fugashi/) written in Python.
Please feel free to use what works best in your use case.

## License and Copyright Notice

mecab_android is a MeCab wrapper for the Android operating system and includes modified MeCab 0.996 source code.
MeCab is copyrighted free software by Taku Kudo `<taku@chasen.org>` and Nippon
Telegraph and Telephone Corporation, and is redistributed under the [BSD
License](./LICENSE.mecab).
