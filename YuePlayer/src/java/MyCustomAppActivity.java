package org.github.nsetzer.example;

import org.github.nsetzer.example.R;
import org.qtproject.qt5.android.bindings.QtActivity;
import org.qtproject.qt5.android.bindings.QtService;
import org.github.nsetzer.example.MyCustomAppService;
import android.content.Intent;
import android.util.Log;
import android.os.Bundle;

public class MyCustomAppActivity extends QtActivity {
    @Override
    public void onCreate(Bundle bundle) {
        super.onCreate(bundle);
        Log.i("Activity", "Starting service!");
        Intent serviceIntent = new Intent(this, MyCustomAppService.class);
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
    @Override
    public void onDestroy() {

        //https://stackoverflow.com/questions/36369845/how-to-stop-service-the-service-when-android-app-has-been-closed
        // https://developer.android.com/reference/android/app/Activity.html
        //https://stackoverflow.com/questions/19568315/how-to-handle-code-when-app-is-killed-by-swiping-in-android/26882533#26882533
        Log.i("Activity", "Service: on Destroy Activity");
        Intent intent = new Intent(MyCustomAppActivity.this, MyCustomAppService.class);
        stopService(intent);

        super.onDestroy();
    }
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
    }
}
