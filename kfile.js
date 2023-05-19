
const path = require('path')
const system = platform === Platform.Windows ? "win32" :
platform === Platform.Linux   ? "linux" :
platform === Platform.FreeBSD   ? "freebsd" :
platform === Platform.OSX     ? "macos" :
platform === Platform.Wasm    ? "wasm" :
platform === Platform.Android ? "android" :
platform === Platform.iOS     ? "ios" :
								   "unknown";

let outputDir = 'Deployment';
const id = process.argv.indexOf("--to");							   
if( id != -1){
	outputDir = process.argv[id+1];
}
let project = new Project('engine');

await project.addProject('../Kinc');

project.addIncludeDir('../plugin_api');
project.addIncludeDir('../Libraries/luau_extensions');
project.addIncludeDir('Sources/api');

project.addIncludeDir('Sources');
project.addFiles(
	'Sources/**',
	'Shaders/**',
	path.resolve('../Libraries/luau_extensions/loadlib.cpp'),
);

// project.addLib('dl');
// project.addLib('m');

project.setDebugDir(outputDir);

project.flatten();

resolve(project);
