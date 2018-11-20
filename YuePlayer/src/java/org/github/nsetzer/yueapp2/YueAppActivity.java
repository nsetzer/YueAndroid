package org.github.nsetzer.yueapp2;

import org.github.nsetzer.yueapp2.R;
import org.qtproject.qt5.android.bindings.QtActivity;
import org.qtproject.qt5.android.bindings.QtService;
import org.github.nsetzer.yueapp2.YueAppService;
import android.content.Intent;
import android.util.Log;
import android.os.Bundle;

import java.util.ArrayList;
import java.util.Locale;
import android.speech.RecognizerIntent;
import android.content.ActivityNotFoundException;
import android.widget.Toast;

public class YueAppActivity extends QtActivity {

    private static YueAppActivity m_instance;
    private final int REQ_CODE_SPEECH_INPUT = 100;

    public YueAppActivity()
    {
        m_instance = this;
    }

    @Override
    public void onCreate(Bundle bundle) {
        super.onCreate(bundle);
        Log.i("YueActivity", "Starting service!");
        Intent serviceIntent = new Intent(this, YueAppService.class);
        startService(serviceIntent);
    }
    @Override
    protected void onResume() {
        super.onResume();
    }
    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
    }
    @Override
    public void onPause() {
        super.onPause();
    }

    public static void makeToast(String message) {
        Toast.makeText(m_instance,
            message,
            Toast.LENGTH_SHORT).show();

    }


    public static void promptSpeechInput() {
        m_instance.promptSpeechInputImpl();
    }

    //http://www.androidhive.info/2014/07/android-speech-to-text-tutorial/
    /**
     * Showing google speech input dialog
     * */
    private void promptSpeechInputImpl() {
        Intent intent = new Intent(RecognizerIntent.ACTION_RECOGNIZE_SPEECH);
        intent.putExtra(RecognizerIntent.EXTRA_LANGUAGE_MODEL,
                RecognizerIntent.LANGUAGE_MODEL_FREE_FORM);
        intent.putExtra(RecognizerIntent.EXTRA_LANGUAGE, Locale.getDefault());
        intent.putExtra(RecognizerIntent.EXTRA_PROMPT, "say something");
        try {
            startActivityForResult(intent, REQ_CODE_SPEECH_INPUT);
        } catch (ActivityNotFoundException a) {
            Toast.makeText(getApplicationContext(),
                    "Speech not Supported",
                    Toast.LENGTH_SHORT).show();
        }
    }

    /**
     * Receiving speech input
     * */
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        switch (requestCode) {
            case REQ_CODE_SPEECH_INPUT: {
                if (resultCode == RESULT_OK && null != data) {

                    ArrayList<String> result = data
                            .getStringArrayListExtra(RecognizerIntent.EXTRA_RESULTS);

                    String msga = "";
                    String msgb = "";
                    if (result.size() > 0)
                        msga = result.get(0);
                    if (result.size() > 1)
                        msgb = result.get(1);

                    JNIActivitySTTResultCallback(msga,msgb);

                }
                break;
            }
        }
        super.onActivityResult(requestCode, resultCode, data);
    }

    @Override
    public void onDestroy() {

        //https://stackoverflow.com/questions/36369845/how-to-stop-service-the-service-when-android-app-has-been-closed
        // https://developer.android.com/reference/android/app/Activity.html
        //https://stackoverflow.com/questions/19568315/how-to-handle-code-when-app-is-killed-by-swiping-in-android/26882533#26882533
        Log.i("YueActivity", "Service: on Destroy Activity");
        Intent intent = new Intent(YueAppActivity.this, YueAppService.class);
        stopService(intent);

        super.onDestroy();
    }

    public static native void JNIActivitySTTResultCallback(String msga, String msgb);

    //@Override
    //protected void onActivityResult(int requestCode, int resultCode, Intent data) {
    //    super.onActivityResult(requestCode, resultCode, data);
    //}
}
