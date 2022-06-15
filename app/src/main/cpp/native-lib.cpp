#include <jni.h>
#include <string>
#include <android/log.h>
#include "UsageEnvironment/include/UsageEnvironment.hh"
#include "BasicUsageEnvironment/include/BasicUsageEnvironment.hh"
#include "liveMedia/include/GenericMediaServer.hh"
#include "liveMedia/include/RTSPServer.hh"
#include "liveMedia/include/H264VideoFileServerMediaSubsession.hh"


#define LOG_TAG "rtsp_server"
#define LOGV(...)  __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
char* url;
RTSPServer* rtspServer;
ServerMediaSession* sms;
TaskScheduler* scheduler;
UsageEnvironment* env_;

extern "C"
JNIEXPORT jstring JNICALL
Java_com_brigittta_live555_rtspService_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_brigittta_live555_rtspService_MainActivity_start(JNIEnv *env, jobject instance, jstring fileName_) {
//    const char *fileName = env->GetStringUTFChars(fileName_, 0);
//
//    // TODO
//
//    env->ReleaseStringUTFChars(fileName_, fileName);
//
//    return env->NewStringUTF(returnValue);
    const char *inputFilename = env->GetStringUTFChars(fileName_, 0);
    FILE *file = fopen(inputFilename, "rb");
    if (!file) {
        LOGE("couldn't open %s", inputFilename);
        //exit(1);
        return NULL;
    }
    fclose(file);

    scheduler = BasicTaskScheduler::createNew();
    env_ = BasicUsageEnvironment::createNew(*scheduler);
    UserAuthenticationDatabase* authDB = NULL;
    // Create the RTSP server:
    rtspServer = RTSPServer::createNew(*env_, 1234, NULL);
    if (rtspServer == NULL) {
        LOGE("Failed to create RTSP server: %s", env_->getResultMsg());
        //exit(1);
        return NULL;
    }
    char const* descriptionString = "Session streamed by \"testOnDemandRTSPServer\"";
    char const* streamName = "v";
    sms  = ServerMediaSession::createNew(*env_, streamName, streamName, descriptionString);
    sms->addSubsession(H264VideoFileServerMediaSubsession::createNew(*env_, inputFilename, True));
    rtspServer->addServerMediaSession(sms);

    url = rtspServer->rtspURL(sms);
    LOGE("%s stream, from the file %s ",streamName, inputFilename);
    LOGE("Play this stream using the URL: %s", url);
//    delete[] url;

    env->ReleaseStringUTFChars(fileName_, inputFilename);
    env_->taskScheduler().doEventLoop(); // does not return
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_brigittta_live555_rtspService_MainActivity_stop(JNIEnv *env, jobject instance) {

//    // TODO
//
//
//    return env->NewStringUTF(returnValue);
    if(rtspServer != NULL) {
        // STEP1, sms;

        // release framesource(S1)
        rtspServer->closeAllClientSessionsForServerMediaSession(sms);
        // release mediasession(S2) and mediasubsession(S3)
        rtspServer->removeServerMediaSession(sms);

        // STEP2, rtspServer(S4);
        Medium::close(rtspServer);

        rtspServer = NULL;
//        tssms = NULL;
    }

// STEP3, env(S5);
    if(env_ != NULL) {
        env_->reclaim();
        env_ = NULL;
    }

// STEP4, scheduler(S6);
    if(scheduler != NULL) {
        delete scheduler;
        scheduler = NULL;
    }
}