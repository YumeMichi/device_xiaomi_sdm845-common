/*
 * Copyright (C) 2018 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package org.mokee.settings.display;

import android.os.Bundle;
import android.view.MenuItem;

import androidx.preference.Preference;
import androidx.preference.Preference.OnPreferenceChangeListener;
import androidx.preference.PreferenceFragment;
import androidx.preference.SwitchPreference;

import org.mokee.settings.R;

import vendor.xiaomi.hardware.displayfeature.V1_0.IDisplayFeature;

public class FlickerFreeSettingsFragment extends PreferenceFragment implements
        OnPreferenceChangeListener {

    private static final String FLICKER_FREE_ENABLE_KEY = "flicker_free_enable";
    private SwitchPreference mFlickerFreePreference;
    private IDisplayFeature mDisplayFeature;

    @Override
    public void onCreatePreferences(Bundle savedInstanceState, String rootKey) {
        addPreferencesFromResource(R.xml.flickerfree_settings);
        getActivity().getActionBar().setDisplayHomeAsUpEnabled(true);
        try {
            mDisplayFeature = IDisplayFeature.getService();
        } catch (Exception e) {
            // Do nothing
        }
        mFlickerFreePreference = (SwitchPreference) findPreference(FLICKER_FREE_ENABLE_KEY);
        mFlickerFreePreference.setEnabled(true);
        mFlickerFreePreference.setOnPreferenceChangeListener(this);
    }

    @Override
    public boolean onPreferenceChange(Preference preference, Object newValue) {
        if (FLICKER_FREE_ENABLE_KEY.equals(preference.getKey())) {
            enableFlickerFree((Boolean) newValue ? 1 : 0);
        }
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        if (item.getItemId() == android.R.id.home) {
            getActivity().onBackPressed();
            return true;
        }
        return false;
    }

    private void enableFlickerFree(int enable) {
        if (mDisplayFeature == null) return;
        try {
            mDisplayFeature.setFeature(0, 20, enable, 0);
        } catch (Exception e) {
            // Do nothing
        }
    }
}
