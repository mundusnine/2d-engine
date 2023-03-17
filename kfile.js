
const system = platform === Platform.Windows ? "win32" :
platform === Platform.Linux   ? "linux" :
platform === Platform.FreeBSD   ? "freebsd" :
platform === Platform.OSX     ? "macos" :
platform === Platform.Wasm    ? "wasm" :
platform === Platform.Android ? "android" :
platform === Platform.iOS     ? "ios" :
								   "unknown";

let project = new Project('engine');

await project.addProject('../Kinc');

project.addIncludeDir('../plugin_api');
project.addIncludeDir('Sources/api');

project.addIncludeDir('Sources');
project.addFiles('Sources/**','Shaders/**');

// project.addLib('dl');
// project.addLib('m');

project.setDebugDir('Deployment');

project.flatten();

resolve(project);
