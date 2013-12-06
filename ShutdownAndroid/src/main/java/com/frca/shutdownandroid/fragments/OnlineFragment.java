package com.frca.shutdownandroid.fragments;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.LinearLayout;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;

import com.frca.shutdownandroid.Connection;
import com.frca.shutdownandroid.HttpTask;
import com.frca.shutdownandroid.NetworkThread;
import com.frca.shutdownandroid.R;

import java.util.List;

/**
 * Created by KillerFrca on 1.12.13.
 */
public class OnlineFragment extends ChildFragment {

    private String currentMessage;
    public OnlineFragment(Connection connection) {
        super(connection);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        LinearLayout layout = (LinearLayout) inflater.inflate(R.layout.fragment_online, container, false);
        for (int i = 0; i < layout.getChildCount(); ++i) {
            LinearLayout child = (LinearLayout) layout.getChildAt(i);
            if (child == null)
                continue;

            for (int y = 0; y < child.getChildCount(); ++y) {
                View item = child.getChildAt(y);
                if (item == null)
                    continue;

                item.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View view) {
                        final NetworkThread thread = getMainActivity().getThread();
                        String str = getResponseForView(view.getId());

                        if (str != null) {
                            thread.setIp(connection.getIp());
                            thread.sendMessage(str, new NetworkThread.OnMessageReceived() {
                                @Override
                                public void messageReceived(String message) {
                                    if (message.equals("OK")) {
                                        Toast.makeText(getActivity(), "Successful!", Toast.LENGTH_LONG).show();
                                    } else
                                        Toast.makeText(getActivity(), "Error!", Toast.LENGTH_LONG).show();
                                }
                            });
                            return;
                        }

                        if (view.getId() == R.id.torrent) {
                            handleTorrentRequest(thread);
                            return;
                        }

                        if (view.getId() == R.id.volume) {
                            handleVolumeRequest(thread);
                            return;
                        }

                        Toast.makeText(getActivity(), "No action specified for view id `" + String.valueOf(view.getId()) + "`", Toast.LENGTH_LONG).show();
                    }
                });
            }
        }

        return layout;
    }

    protected String getResponseForView(int resId) {
        switch (resId) {
            case R.id.turn_off: return "TURN_OFF";
            case R.id.restart:  return "RESTART";
            case R.id.lock:     return "LOCK";
            case R.id.sleep:    return "SLEEP";
            //case R.id.volume:   return "GET_VOLUME";
        }

        return null;
    }

    private void handleTorrentRequest(final NetworkThread thread) {
        new HttpTask(getActivity(), "http://www.serialzone.cz/watchlist/", new HttpTask.OnHandled() {
            @Override
            public void call(List<String> list) {
                if (list == null)
                    return;
                thread.setIp(connection.getIp());
                final AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
                builder.setTitle("Data").setPositiveButton(android.R.string.ok, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialogInterface, int i) {
                        dialogInterface.dismiss();
                    }
                });

                final AlertDialog dialog = builder.create();
                currentMessage = "";

                StringBuilder sb = new StringBuilder();
                sb.append("TORRENT ");
                for (String ser : list) {
                    ser = ser.replaceAll("\\.", " ");
                    ser = ser.replaceAll("-", " ");
                    sb.append("\""+ser+"\", ");
                }

                thread.sendMessage(sb.toString(), new NetworkThread.OnMessageReceived() {
                    @Override
                    public void messageReceived(String message) {
                        if (!currentMessage.equals(""))
                            currentMessage += "\n";

                        currentMessage += message;

                        dialog.setMessage(currentMessage);
                        if (!dialog.isShowing())
                            dialog.show();
                    }
                });
            }
        }).execute();
    }

    private void handleVolumeRequest(final NetworkThread thread) {
        thread.setIp(connection.getIp());
        thread.sendMessage("GET_VOLUME", new NetworkThread.OnMessageReceived() {
            @Override
            public void messageReceived(String message) {
                float value = Float.valueOf(message);
                AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());

                LayoutInflater inflater = (LayoutInflater) getActivity().getSystemService(Context.LAYOUT_INFLATER_SERVICE);
                final View view = inflater.inflate(R.layout.dialog_seek, null);

                final TextView nameField = (TextView) view.findViewById(R.id.text1);
                final TextView valueField = (TextView) view.findViewById(R.id.text2);
                nameField.setText("Current volume:");
                SeekBar bar = (SeekBar) view.findViewById(R.id.seek);
                bar.setMax(1000);
                final LimitedExecutor runnable = new LimitedExecutor(100);
                bar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
                    @Override
                    public void onProgressChanged(final SeekBar seekBar, final int i, boolean b) {
                        runnable.execute(new Runnable() {
                            @Override
                            public void run() {
                                valueField.setText(String.valueOf(i/10.f) + "%");
                                float outValue = seekBar.getProgress()/1000.f;
                                thread.sendMessage("SET_VOLUME " + String.valueOf(outValue), NetworkThread.doNothingOnMessage);
                            }
                        });
                    }

                    @Override
                    public void onStartTrackingTouch(SeekBar seekBar) {

                    }

                    @Override
                    public void onStopTrackingTouch(SeekBar seekBar) {
                        valueField.setText(String.valueOf(seekBar.getProgress()/10.f) + "%");
                        float outValue = seekBar.getProgress()/1000.f;
                        thread.sendMessage("SET_VOLUME " + String.valueOf(outValue), NetworkThread.doNothingOnMessage);
                    }
                });
                bar.setProgress((int) (value * 1000));
                builder.setTitle("Set Volume")
                    .setView(view)
                    .setPositiveButton(android.R.string.ok, new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialogInterface, int i) {
                            dialogInterface.dismiss();
                        }
                    });

                builder.create().show();;
            }
        });
    }
}
