# ColorPal - a simple wrapper for [Little-CMS](http://littlecms.com)

### Building

First install the Android NDK (last used `android-ndk-r12b`)

```bash
git submodule init
git submodule update
cd colorpal
ndk-build
cd ..
./gradlew build publish
```

This will produce the aar artifact in `colorpal/build/repo/com/pushd/colorpal/<Version>/colorpal-<Version>.aar`

