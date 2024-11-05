'use strict'

const {tkrzw} = require('bindings')('tkrzw-node')
module.exports = tkrzw;

var fs = require('fs');
let tkrzw_config = fs.readFileSync('./tkrzw_config.json', 'utf8');


//const db1 = new tkrzw(JSON.parse(tkrzw_config), "YaHeidar.tkh");
const db1 = new tkrzw(require("./tkrzw_config.json"), "YaHeidar.tkh");
Promise.allSettled([
db1.set("Heidar(a)", "Amir-al-momenin").then(()=>console.log("SET1 DONE!")),
db1.set("Heidar(b)", "Amir-al-momenin").then(()=>console.log("SET2 DONE!")),
db1.getSimple("Heidar(b)", "Key not_set").then((val)=>console.log("GET3: ",val)),
db1.getSimple("Heidar(a)", "Key not_set").then((val)=>console.log("GET1: ",val)),
db1.getSimple("non existant key", "Key not_set").then((val)=>console.log("GET2: ",val)).catch((val)=>console.log("GET2: ",val)),
db1.shouldBeRebuilt().then(()=>console.log("shouldBeRebuilt: Yes it should")).catch((val)=>console.log("shouldBeRebuilt: ",val))
]).then(/*()=>db1.close()*/);

db1.rebuild(require("./tkrzw_rebuild_config.json"))