const fs = require('fs');
const path = require('path');
const PROJECT_NAME = path.basename(__dirname).toUpperCase();
// child.execSync("cd Tools/node-libclang && npm install && cd ..");
const {
    ChildVisitResult,
    CursorKind,
    Index,
    TranslationUnit,
    TypeKind,
} = require("./Tools/node-libclang");
const { isObject } = require("util");
const { Cursor } = require('./Tools/node-libclang');
const { Type } = require('./Tools/node-libclang');

function printObj(obj,indent){
    for (const key in obj) {
        if (obj.hasOwnProperty(key)) {
            console.log(indent+key + ': ' + obj[key]);
            if(isObject(obj[key]) || obj[key].type){
                printObj(obj[key],indent+" ")
            }
        }
    }
};

const base_types = ["Void","UInt",    "Int","Float","Double","Char_S","UChar"];
const convert_to = ["void","unsigned","int","float","double","char"  ,"unsigned char"];

const func_prefix_avoid = ["lua","kr_","module_reload"]

const files = ["Sources/system.h","Sources/renderer.h"]

function convertTypename(name){
    let nom = name.split(' ').join('');
    for(let i = 0; i < base_types.length; ++i){
        if(nom === base_types[i]){
            return convert_to[i];
        }
    }
    return name;
};

function getConstantArrayType(arr_type){
    const elementType = arr_type.arrayElementType.spelling;
    const numElements = arr_type.arraySize;
    return `${convertTypename(elementType)}:[${numElements}]`
}

// printObj(TypeKind,"");
// printObj(CursorKind,"");

function getFunctionDef(fcursor,ftype){
    let out = {};
    const retIsPointer = ftype.result.kind === TypeKind.Pointer; 
    let returnType = retIsPointer ? ftype.result.pointeeType : ftype.result;
    returnType = returnType.kind === TypeKind.Typedef ? returnType.declaration : returnType;
    const p = retIsPointer ? "*" : " ";

    out.returnType = `${convertTypename(returnType.spelling)}${p}`;
    out.funcName =  fcursor.spelling;
    out.args = [];
    const decl = fcursor;
    const parms = [];
    decl.visitChildren(function (_parent) {
        if (this.kind === CursorKind.ParmDecl){
            parms.push(this.spelling);
        }
        return ChildVisitResult.Continue;
    });

    let len = ftype.numberOfArguments;
    for(let i =0; i < len;++i){
        const arg = ftype.getArgument(i);
        const argIsPointer = arg.kind === TypeKind.Pointer;
        let argumentType = argIsPointer ? arg.pointeeType : arg;
        argumentType = argumentType.kind === TypeKind.Typedef ? argumentType.declaration : argumentType;
        const ap = argIsPointer ? "*" : " ";
        const argName = parms.length === ftype.numberOfArguments ? parms[i] : arg.spelling;
        let type_spelling = convertTypename(argumentType.spelling) + ap + ":" + argName; 
        if(argumentType.kind === TypeKind.FunctionProto){
            let func_def = getFunctionDef(arg.declaration,argumentType)
            type_spelling = ` ${func_def.returnType} (*${parms[i]})(`
            for(let y = 0; y < func_def.args.length;++y){
                type_spelling += func_def.args[y].split(':')[0];
                if(y+1 < func_def.args.length){
                    type_spelling +=', '
                }
            }
            type_spelling +=') :' 
        }
        // if(i +1 < len){
        //     type_spelling += ', ';
        // }
        out.args.push(type_spelling);
    }
    if(ftype.numberOfArguments === 0){
        out.args.push("void");
    }

    return out;
}

let content = `#pragma once
/**
IMPORTANT: Define in only one .c or .cpp file ${PROJECT_NAME}_IMPL
The luauc module API is quite simple. Any shared library can be a module file, so long
as it has an entrypoint that looks like the following, where xxxxx is your gameplay dynamic library name:
#define ${PROJECT_NAME}_IMPL
#include "${PROJECT_NAME.toLowerCase()}_api.h"
int luaopen_luauc_xxxxx(void* L, void* LUAUC) {
    ${PROJECT_NAME.toLowerCase()}_init(LUAUC);
    ...
    return 1;
}
Two functions will be called by default for modules i.e. module_init and module_quit.
You need to define these functions in your code.
In linux, to compile this file, you'd do: 'gcc -o xxxxx.so -shared xxxxx.c'. Simple!
IMPORTANT: **This file was automatically generated. DO NOT MODIFY DIRECTLY.**
**/

#include "stdio.h"

int module_init(void);
int module_quit(void);
\n`;
let impl_content = `
void ${PROJECT_NAME.toLowerCase()}_init(void* LUAUC);
#ifdef ${PROJECT_NAME}_IMPL
#define IMPORT_SYMBOL(name, ret, ...) name = (name = (ret (*) (__VA_ARGS__)) symbol(#name), name == NULL ? &__fallback_##name : name)\n`;
let func_impl = `void ${PROJECT_NAME.toLowerCase()}_init(void* LUAUC){
    void* (*symbol)(const char *) = (void* (*) (const char *)) LUAUC;
`;

for(fi=0; fi < files.length;++fi ){
    const file = files[fi];
    console.log("Working on file: "+file)
    const index = new Index();
    const tu = TranslationUnit.fromSource(index,file, []);

    // NOTE: the child visitor is bound to the wrapped libclang AST cursor, so avoid () => {} fat arrow functions.
    tu.cursor.visitChildren(function (_parent) {
        switch (this.kind) {
            case CursorKind.TypedefDecl:
                const currCursor = this.typedefType.declaration;
                const type = currCursor.kind === CursorKind.StructDecl ? 'struct' : currCursor.kind === CursorKind.UnionDecl ? 'union': currCursor.kind === CursorKind.EnumDecl ? 'enum' : 'error';
                // console.log(currCursor.spelling);
                content += `typedef ${type} ${this.spelling} ${this.spelling};\n`;
                impl_content += `${type} ${this.spelling}{\n`
                let visitFunc = function (_parent) {};
                if(currCursor.kind === CursorKind.StructDecl || currCursor.kind === CursorKind.UnionDecl){
                    visitFunc = function (_parent) {
                        if (this.kind === CursorKind.FieldDecl) {
                            const fieldIsPointer = this.type.kind === TypeKind.Pointer;
                            let fieldType = fieldIsPointer ? this.type.pointeeType : this.type;
                            // console.log(`Field named: ${this.spelling} of kind: ${fieldType.kind}`)
                            fieldType = fieldType.kind === TypeKind.Typedef ? fieldType.declaration : fieldType;
                            const ap = fieldIsPointer ? "*" : "";
                            let type_spelling = convertTypename(fieldType.spelling);
                            let arr = "";
                            if(fieldType.kind === TypeKind.ConstantArray){
                                type_spelling = getConstantArrayType(fieldType);
                                let info = type_spelling.split(':');
                                type_spelling = info[0]; 
                                arr = info[1];
                            }
                            impl_content += "    " + type_spelling + ap + " "+ this.spelling + arr + ";\n";
                            // console.log("Field Name: " + this.spelling);
                            // console.log("Field Type: " + this.type.spelling);
                        }
                        return ChildVisitResult.Continue;
                    };
                }
                if(currCursor.kind === CursorKind.EnumDecl){
                    visitFunc = function (_parent) {
                        if (this.kind === CursorKind.EnumConstantDecl) {
                            // console.log("Enumerator: " + this.spelling);
                            // console.log("Value: " + enumValue.toString());
                            const enumValue = this.enumValue;
                            impl_content += "    " + this.spelling + "="+ enumValue.toString()+ ",\n";
                        }
                        return ChildVisitResult.Continue;
                    };
                }
                // Retrieve fields of RenImage struct
                currCursor.visitChildren(visitFunc);
                impl_content += '};\n'
                break;
            case CursorKind.FunctionDecl:
                // console.log("Function: " + this.spelling);
                const funcname = this.spelling
                let avoid = false;
                for(let y=0; y < func_prefix_avoid.length;++y){
                    if(funcname.startsWith(func_prefix_avoid[y])){
                        avoid = true;
                        break;
                    }
                }
                if(avoid)
                    break;
                let func_def = getFunctionDef(this,this.type);
                content += `${func_def.returnType} (*${func_def.funcName})(`;
                //static 	lua_State*	__fallback_	(lua_Alloc f, void* ud) { fputs("warning: lua_newstate is a stub\n", stderr);  }
                impl_content += `static ${func_def.returnType} __fallback_${func_def.funcName}(`
                func_impl += `    IMPORT_SYMBOL(${func_def.funcName},${func_def.returnType}`

                for(let i =0; i < func_def.args.length;++i){
                    const arg_arr = func_def.args[i].split(':');
                    const line = arg_arr.length > 1 ? `${arg_arr[0]} ${arg_arr[1]}` :  arg_arr[0];
                    content += line;
                    impl_content += line;
                    func_impl += ', ' + arg_arr[0];
                    if(i +1 < func_def.args.length){
                        content += ', ';
                        impl_content += ', ';
                    }
                }
                content += ');\n'
                impl_content += `) { fputs("warning: ${func_def.funcName} is a stub\\n",stderr); }\n`
                func_impl +=');\n';
                break;
            default:
                break;
        }

        return ChildVisitResult.Continue;
    });
    // NOTE: remember to clean up.
    tu.dispose();
    index.dispose();
}

func_impl += "}"
impl_content += `${func_impl}\n\n#endif//${PROJECT_NAME}_IMPL`

fs.writeFileSync(`include/${PROJECT_NAME.toLowerCase()}_api.h`,content + "\n\n"+ impl_content);