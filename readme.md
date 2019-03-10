[![Build Status](https://travis-ci.org/BrewBlox/brewblox-firmware.svg?branch=feature%2Ftravis-ci)](https://travis-ci.org/BrewBlox/brewblox-firmware)

This is the main source code repository for the firmware on the BrewBlox brewery controller.


## Getting started
End users will not have to compile the firmware themselves.
We provide a docker image to flash releases.


## Changelog
Please see our GitHub release for the change log


## License
Unless stated elsewhere, file headers or otherwise, all files herein are licensed under an GPLv3 license. For more information, please read the LICENSE file.


## Contribute
Contributions to our firmware are very welcome. Please contact us first via our [community forum](https://community.brewpi.com/) to discuss what you want to code to make sure that it aligns with our road map.

Please send pull requests against the develop branch. We can only accept your pull request if you have signed our [Contributor License Agreement (CLA)](http://www.brewpi.com/cla/).

## Development Tools 
We recommend Visual Studio Code for development.
Microsoft Intellisense doesn't work well for this codebase, so we recommend ccls instead.
Follow the install instructions here:

https://github.com/MaskRay/ccls

For automatic formatting, we use clang-format. Install it on your system and install the clang-format plugin in vscode.
