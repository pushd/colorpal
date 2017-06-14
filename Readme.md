# ColorPal - simple icc profile color correction for android

ColorPal is powered by [Little-CMS](http://littlecms.com)

[![](https://jitpack.io/v/pushd/colorpal.svg)](https://jitpack.io/#pushd/colorpal)

### Installation

1. Add the JitPack repository to your build file

Add it in your root build.gradle at the end of repositories:

```
	allprojects {
		repositories {
			...
			maven { url 'https://jitpack.io' }
		}
	}
```

2. Add the dependency

```
	dependencies {
	        compile 'com.github.pushd:colorpal:0.1.7'
	}
```

### Usage

Without renderscript:

```java
import com.pushd.colorpal.ColorCorrector;
...

ColorCorrector corrector = new ColorCorrector();
corrector.loadDisplayICCProfile("/path/to/profile.icc");

Bitmap mybitmap = ...;
corrector.correctBitmap(mybitmap);
// mybitmap has been updated in-place
```

With a renderscript 3DLUT:

```java
import com.pushd.colorpal.ColorCorrector;
...

ColorCorrector corrector = new ColorCorrector();
corrector.loadDisplayICCProfile("/path/to/profile.icc");

RenderScript rs = RenderScript.create(this);
ScriptIntrinsic3DLUT lut = corrector.create3DLUT(rs);

Bitmap original = ...;
Bitmap corrected = original.copy(Bitmap.Config.ARGB_8888, false);

Allocation input = Allocation.createFromBitmap(rs, original);
Allocation output = Allocation.createFromBitmap(rs, corrected);

lut.forEach(input, output);
output.copyTo(corrected);

// 'corrected' now contains the corrected bitmap
```

### Development

1. install the Android NDK (last used `android-ndk-r12b`)

2. add `ndk.dir=<your-path-to-ndk>` in local.properties

3. Run:

```bash
git submodule init
git submodule update
./gradlew assemble
```
