package lib.ysflight;

import android.util.Log;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.View;

import java.io.File;
import java.util.Timer;
import java.util.TimerTask;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.opengles.GL10;

public class YsGLES2View extends GLSurfaceView {
    static {
        System.loadLibrary("ysgles2lib");
    }
    public native void YsInitialize();
    public native boolean YsInterval();
    public native void YsDraw();
    public native void YsOnPause();
    public native void YsOnResume();
    public native void YsNotifyWindowSize(int wid,int hei);
    public native void YsNotifyTouch(int xy[]);

    Timer timer;
    IntervalFunc interval;
    int realWid,realHei,virtualWid,virtualHei;

    public YsGLES2View(Context context)
    {
        super(context);

        // getHolder().addCallback(this);  Do I need this?
        realWid=1;
        realHei=1;
        virtualWid=1;
        virtualHei=1;

        setEGLContextFactory(new ContextFactory());
        setEGLConfigChooser(new ConfigChooser());
        setRenderer(new Renderer(this));
        setOnTouchListener(new TouchHandler(this));
        setPreserveEGLContextOnPause(Boolean.TRUE);

        // setRenderMode is effective only after setRenderer.
        setRenderMode(RENDERMODE_WHEN_DIRTY);

        // Timer called while drawing???  Why is Java this broken???
        // timer=new Timer();
        // timer.schedule(new IntervalTimer(this),0,100);

        interval=new IntervalFunc(this);
    }

    public String GetFilesDir()
    {
        File filesDir=getContext().getFilesDir();
        String path=filesDir.getAbsolutePath();
        Log.i("Files_AbsoluteFileDir",path);
        return path;
    }

    public String GetCacheDir()
    {
        File cacheDir=getContext().getCacheDir();
        String path=cacheDir.getAbsolutePath();
        Log.i("Cache_AbsoluteFileDir",path);
        return path;
    }

    @Override public void surfaceCreated(SurfaceHolder holder)
    {
        int wid=getWidth();
        int hei=getHeight();
        realWid=wid;
        realHei=hei;
        if(wid<hei && 1024<hei)
        {
            wid=1024*wid/hei;
            hei=1024;
            holder.setFixedSize(wid,hei);
        }
        else if(hei<wid && 1024<wid)
        {
            hei=1024*hei/wid;
            wid=1024;
            holder.setFixedSize(wid,hei);
        }
        virtualWid=wid;
        virtualHei=hei;
        super.surfaceCreated(holder);
    }

    private static class TouchHandler implements OnTouchListener
    {
        YsGLES2View owner;
        TouchHandler(YsGLES2View ownerIn)
        {
            owner=ownerIn;
        }
        @Override
        public boolean onTouch(View v,MotionEvent evt)
        {
            Log.i("onTouch","onTouch");
            if(MotionEvent.ACTION_CANCEL!=evt.getAction() &&
                    MotionEvent.ACTION_UP!=evt.getAction()) {
                int c = evt.getPointerCount();
                int[] xy = new int[c * 2];
                for (int i = 0; i < c; ++i) {
                    xy[i * 2] = (int) evt.getX(i)*owner.virtualWid/owner.realWid;
                    xy[i * 2 + 1] = (int) evt.getY(i)*owner.virtualHei/owner.realHei;
                }
                owner.YsNotifyTouch(xy);
            }
            else
            {
                int [] xy=new int [0];
                owner.YsNotifyTouch(xy);
            }
            return true;
        }
    }

    private static class IntervalFunc implements Runnable
    {
        YsGLES2View owner;
        IntervalFunc(YsGLES2View ownerIn)
        {
            owner=ownerIn;
        }
        public void run()
        {
            boolean needRedraw=false;

            Log.i("interval", "interval");
            needRedraw=owner.YsInterval();

            if(true!=needRedraw)
            {
                owner.queueEvent(this);
                // postDelayed, post will run it on the UIThread.
                // I am not asking GLSurfaceView for the multi threading.  I control the threading.
            }
            else
            {
                owner.requestRender();
            }
        }
    }

    private static class IntervalTimer extends TimerTask
    {
        YsGLES2View owner;
        IntervalTimer(YsGLES2View ownerIn)
        {
            owner=ownerIn;
        }
        public void run()
        {
            Log.i("timer", "timer");
            owner.YsInterval();
        }
    }

    private static class ContextFactory implements GLSurfaceView.EGLContextFactory
    {
        private static int EGL_CONTEXT_CLIENT_VERSION=0x3098;  // I have no idea what it is.
        public EGLContext createContext(EGL10 egl,EGLDisplay disp,EGLConfig cfg)
        {
            int attrib[]={EGL_CONTEXT_CLIENT_VERSION,2,EGL10.EGL_NONE};
            return egl.eglCreateContext(disp,cfg, EGL10.EGL_NO_CONTEXT,attrib);
        }
        public void destroyContext(EGL10 egl,EGLDisplay disp,EGLContext context)
        {
            egl.eglDestroyContext(disp,context);
        }
    }

    private static class ConfigChooser implements GLSurfaceView.EGLConfigChooser
    {
        public EGLConfig chooseConfig(EGL10 egl,EGLDisplay disp)
        {
            int configAttrib[]=
                    {
                            EGL10.EGL_RED_SIZE,4,
                            EGL10.EGL_GREEN_SIZE,4,
                            EGL10.EGL_BLUE_SIZE,4,
                            EGL10.EGL_RENDERABLE_TYPE,4,
                            // EGL10.EGL_DEPTH_SIZE,24,
                            EGL10.EGL_NONE
                    };

            int numConfig[]={0};
            egl.eglChooseConfig(disp,configAttrib,null,0,numConfig);
            if(0<numConfig[0])
            {
                EGLConfig[] config=new EGLConfig[numConfig[0]];
                egl.eglChooseConfig(disp,configAttrib,config,numConfig[0],numConfig);
                for(EGLConfig c : config)
                {
                    int depth[]={0},stencil[]={0};
                    egl.eglGetConfigAttrib(disp,c,EGL10.EGL_DEPTH_SIZE,depth);
                    egl.eglGetConfigAttrib(disp,c,EGL10.EGL_STENCIL_SIZE,stencil);
                    if(16<=depth[0] && 1<=stencil[0])
                    {
                        return c;
                    }
                }
            }
            else
            {
                Log.e("YsGLES2View","EGL Config Chooser: Cannot find a config.");
            }
            return null;
        }
    }

    private static class Renderer implements GLSurfaceView.Renderer
    {
        YsGLES2View owner;
        public Renderer(YsGLES2View ownerIn)
        {
            owner=ownerIn;
        }
        public void onDrawFrame(GL10 gl)
        {
            Log.i("onDrawFrame", "onDrawFrame");
            // gl.glClearColor(0, 1, 1, 0);
            // gl.glClear(gl.GL_COLOR_BUFFER_BIT);
            owner.YsDraw();

            owner.queueEvent(owner.interval);
            // postDelayed, post will run it on the UIThread.
            // I am not asking GLSurfaceView for the multi threading.  I control the threading.
        }
        public void onSurfaceChanged(GL10 gl,int wid,int hei)
        {
            owner.YsNotifyWindowSize(wid,hei);
        }
        public void onSurfaceCreated(GL10 gl,EGLConfig cfg)
        {
            int wid=owner.getWidth();
            int hei=owner.getHeight();
            owner.YsNotifyWindowSize(wid,hei);
            gl.glClearColor(1, 1, 1, 0);
            owner.YsInitialize();
            owner.queueEvent(owner.interval);
            // postDelayed, post will run it on the UIThread.
            // I am not asking GLSurfaceView for the multi threading.  I control the threading.
        }
    }
}
