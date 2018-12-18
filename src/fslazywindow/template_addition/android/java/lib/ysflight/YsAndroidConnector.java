package lib.ysflight;

import android.content.Context;
import android.media.MediaScannerConnection;
import android.util.Log;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;

public class YsAndroidConnector {
    static Context ctxCache;

    public YsAndroidConnector(Context ctxIn)
    {
        ctxCache=ctxIn;
    }

    static public String GetFilesDir()
    {
        File filesDir=ctxCache.getFilesDir();
        String path=filesDir.getAbsolutePath();
        Log.i("Files_AbsoluteFileDir",path);
        return path;
    }

    static public String GetCacheDir()
    {
        File cacheDir=ctxCache.getCacheDir();
        String path=cacheDir.getAbsolutePath();
        Log.i("Cache_AbsoluteFileDir",path);
        return path;
    }

    static public String GetExternalFilesDir()
    {
        File dir=ctxCache.getExternalFilesDir(null);
        String path=dir.getAbsolutePath();
        Log.i("ExternalFilesDir",path);

        File dog=new File(dir,"ys_claiming_territory.txt");
        try {
            OutputStream fp = new FileOutputStream(dog);
            fp.write(100);
            fp.close();

            // Until MediaScanner scans the file, it is not visible to the outside.
            // https://developer.android.com/reference/android/os/Environment#getExternalStoragePublicDirectory(java.lang.String)
            MediaScannerConnection.scanFile(ctxCache,
                    new String[]{dog.toString()},
                    null,
                    null);
        }
        catch(IOException e)
        {
            Log.i("Ys","File Write Error!");
        }




        return path;
    }
}

