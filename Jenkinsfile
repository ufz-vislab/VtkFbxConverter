#!/usr/bin/env groovy
@Library('jenkins-pipeline@1.0.3') _

def configure = new ogs.configure()
def build = new ogs.build()
def helper = new ogs.helper()

timestamps {

node("win1") {
    checkout scm
    withEnv(helper.getEnv(this, 'x64', '12')) {
      configure.win(
        cmakeOptions: " -DParaView_DIR=E:/bilke/pv/build-pv/superbuild/paraview/build ",
        script: this,
        sourceDir: "./",
        useConan: false
      )
      build.win(target: '',script: this)
    }
    archive '**/*.dll'
    bat 'copy build\\pv_plugin\\FbxExporter.dll C:\\paraview\\Plugins-5.2.0'
}

} // timestamps
