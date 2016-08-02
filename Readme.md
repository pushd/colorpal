# ColorPal - a simple wrapper for [Little-CMS](http://littlecms.com)

### Building

First install the Android NDK (last used `android-ndk-r12b`)

```bash
git submodule init
git submodule update
cd colorpal
ndk-build
cd ..
./gradlew assembleRelease
```

This will produce the aar artifact in `colorpal/build/outputs/aar/colorpal-release.aar`

