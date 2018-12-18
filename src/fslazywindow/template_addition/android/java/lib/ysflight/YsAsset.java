package lib.ysflight;

import android.content.Context;
import android.content.res.AssetManager;
import android.util.Log;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

/**
 * Created by soji on 5/18/2018.
 */

// Reference https://gist.github.com/tylerchesley/6198074
    // This example implies that it can identify a name is a file if assetManager.list returns zero length.
    // But the official document doesn't seem to say such a thing.  Is it an official behavior?
    // Or can it change in the future?

public class YsAsset {
    public YsAsset(AssetManager asm, Context ctx)
    {
        if(true!=IsUpToDate(asm,ctx))
        {
            MakeUseful(asm, "", ctx);
        }
    }

    private String GetFilesPath(Context ctx)
    {
        File filesDir = ctx.getFilesDir();
        return filesDir.getAbsolutePath();
    }

    private boolean IsUpToDate(AssetManager asm,Context ctx)
    {
        try {
            String timeStampFName = "ys_timestamp.txt";
            InputStream srcInStream = asm.open(timeStampFName);
            String filesPath = GetFilesPath(ctx);
            InputStream dstInStream = new FileInputStream(filesPath + "/" + timeStampFName);

            while(true)
            {
                int src=srcInStream.read();
                int dst=dstInStream.read();

                String s=String.format("Code %d %d",src,dst);
                Log.i("YsAsset",s);

                if(src!=dst)
                {
                    Log.i("YsAsset","Time-Stamp file different.");

                    srcInStream.close();
                    dstInStream.close();
                    return false;
                }

                if(src<0)
                {
                    break;
                }
            }
            srcInStream.close();
            dstInStream.close();
            return true;
        }
        catch(IOException e)
        {
            Log.i("YsAsset","Failed to open time-stamp file.");
            return false;
        }
    }

    private void MakeUseful(AssetManager asm,String subdir,Context ctx)
    {
        Log.i("YsAsset","Looking into:"+subdir);
        try
        {
            String fName[]=asm.list(subdir);
            String filesPath = GetFilesPath(ctx);
            if(0==fName.length) // Can I say it is a file?  Is it an officially defined behavior?
            {
                if(""!=subdir) // Empty asset?
                {
                    String oFName = filesPath + "/" + subdir;
                    String iFName = subdir;
                    Log.i("YsAsset","Expanding "+iFName + " to " + oFName);

                    InputStream inStream = asm.open(iFName);
                    OutputStream outStream = new FileOutputStream(oFName);
                    byte[] buf = new byte[1024 * 1024];
                    int nRead;
                    while (0 < (nRead = inStream.read(buf))) {
                        outStream.write(buf,0,nRead);
                    }
                    inStream.close();
                    outStream.flush(); // Necessary?
                    outStream.close();
                }
            }
            else
            {
                String fulPath = filesPath + "/" + subdir;

                File dir=new File(fulPath);
                if(Boolean.TRUE!=dir.exists())
                {
                    dir.mkdir();
                }

                for(int i=0; i<fName.length; ++i)
                {
                    String nextSubdir;
                    if(""!=subdir)
                    {
                        nextSubdir=subdir+"/"+fName[i];
                    }
                    else
                    {
                        nextSubdir=fName[i];
                    }
                    MakeUseful(asm,nextSubdir,ctx);
                }
            }
        }
        catch(IOException e)
        {
            Log.e("YsAsset","Asset could not be made useful.  Remains useless.",e);
        }
    }
}
