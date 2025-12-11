/*
 * Copyright 2024 Punch Through Design LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.punchthrough.blestarterappandroid

import android.annotation.SuppressLint
import android.app.AlertDialog
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothGattCharacteristic
import android.os.Bundle
import android.view.MenuItem
import androidx.appcompat.app.AppCompatActivity
import com.punchthrough.blestarterappandroid.ble.ConnectionEventListener
import com.punchthrough.blestarterappandroid.ble.ConnectionManager
import com.punchthrough.blestarterappandroid.ble.ConnectionManager.parcelableExtraCompat
import com.punchthrough.blestarterappandroid.ble.isWritable
import com.punchthrough.blestarterappandroid.ble.isWritableWithoutResponse
import com.punchthrough.blestarterappandroid.ble.toHexString
import com.punchthrough.blestarterappandroid.databinding.ActivityBleOperationsBinding
import java.text.SimpleDateFormat
import java.util.Date
import java.util.Locale
import java.util.UUID
import timber.log.Timber

// Define the UUID of the characteristic you want to write "ON"/"OFF" to
private val TARGET_WRITE_CHARACTERISTIC_UUID =
    UUID.fromString("A495FF20-C5B5-4B44-B512-1370F02D74DE") // <-- CHECK/REPLACE THIS UUID

class BleOperationsActivity : AppCompatActivity() {

    private lateinit var binding: ActivityBleOperationsBinding
    private val device: BluetoothDevice by lazy {
        intent.parcelableExtraCompat(BluetoothDevice.EXTRA_DEVICE)
            ?: error("Missing BluetoothDevice from MainActivity!")
    }

    // Date formatter kept for logging to Logcat
    private val dateFormatter = SimpleDateFormat("MMM d, HH:mm:ss", Locale.US)

    // Fetch all characteristics to find the target one
    private val characteristics by lazy {
        ConnectionManager.servicesOnDevice(device)?.flatMap { service ->
            service.characteristics ?: listOf()
        } ?: listOf()
    }

    // Helper to find the target characteristic once
    private val targetWriteCharacteristic: BluetoothGattCharacteristic? by lazy {
        characteristics.firstOrNull { it.uuid == TARGET_WRITE_CHARACTERISTIC_UUID }
    }
    @SuppressLint("MissingPermission")
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        ConnectionManager.registerListener(connectionEventListener)

        binding = ActivityBleOperationsBinding.inflate(layoutInflater)

        setContentView(binding.root)
        supportActionBar?.apply {
            setDisplayHomeAsUpEnabled(true)
            setDisplayShowTitleEnabled(true)
            title = device.name ?: "BLE Control" // Use device name as title
        }

        // Setup Button Listeners
        binding.writeOnButton.setOnClickListener {
            writeCommand("ON")
        }
        binding.writeOffButton.setOnClickListener {
            writeCommand("OFF")
        }

        // Log that the activity is ready (MTU/Characteristics setup implicitly handled by ConnectionManager)
        log("Connected and ready for commands.")
    }

    override fun onDestroy() {
        ConnectionManager.unregisterListener(connectionEventListener)
        ConnectionManager.teardownConnection(device)
        super.onDestroy()
    }

    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        when (item.itemId) {
            android.R.id.home -> {
                onBackPressed()
                return true
            }
        }
        return super.onOptionsItemSelected(item)
    }

    // --- FUNCTION TO HANDLE WRITE COMMANDS ---
    // --- FUNCTION TO HANDLE WRITE COMMANDS ---
    @SuppressLint("MissingPermission") // <-- ADD THIS ANNOTATION
    private fun writeCommand(command: String) {
        val characteristic = targetWriteCharacteristic
        if (characteristic == null) {
            log("Error: Target characteristic ${TARGET_WRITE_CHARACTERISTIC_UUID} not found on device.")
            return
        }

        if (!characteristic.isWritable() && !characteristic.isWritableWithoutResponse()) {
            log("Error: Target characteristic ${TARGET_WRITE_CHARACTERISTIC_UUID} is not writable.")
            return
        }

        val payload = command.stringToBytes()
        log("Sending '$command' command to ${characteristic.uuid}: ${payload.toHexString()}")
        ConnectionManager.writeCharacteristic(device, characteristic, payload)
    }
// --- END WRITE FUNCTION ---
    // --- END WRITE FUNCTION ---


    // Simplified log function (now only writes to Logcat/Timber)
    @SuppressLint("SetTextI18n")
    private fun log(message: String) {
        val formattedMessage = "${dateFormatter.format(Date())}: $message"
        // Log to Timber (Logcat) instead of a TextView
        Timber.i(formattedMessage)
    }

    private val connectionEventListener by lazy {
        ConnectionEventListener().apply {
            onDisconnect = {
                runOnUiThread {
                    AlertDialog.Builder(this@BleOperationsActivity)
                        .setTitle("Disconnected")
                        .setMessage("Disconnected from device.")
                        .setPositiveButton("OK") { _, _ -> onBackPressed() }
                        .show()
                }
            }

            // Keep logging for status feedback, even though the UI log is gone
            onCharacteristicWrite = { _, characteristic ->
                log("Wrote successfully to ${characteristic.uuid}")
            }
            onMtuChanged = { _, mtu ->
                log("MTU updated to $mtu")
            }
            onCharacteristicRead = { _, characteristic, value ->
                log("Read from ${characteristic.uuid}: ${value.toHexString()}")
            }
            onCharacteristicChanged = { _, characteristic, value ->
                log("Value changed on ${characteristic.uuid}: ${value.toHexString()}")
            }
            onNotificationsEnabled = { _, characteristic ->
                log("Enabled notifications on ${characteristic.uuid}")
            }
            onNotificationsDisabled = { _, characteristic ->
                log("Disabled notifications on ${characteristic.uuid}")
            }
        }
    }

    // Existing function for Hex conversion (kept for completeness, though not strictly needed here)
    private fun String.hexToBytes() =
        this.chunked(2).map { it.uppercase(Locale.US).toInt(16).toByte() }.toByteArray()

    /**
     * Converts a string to a ByteArray using the platform's default charset (usually UTF-8 on Android).
     */
    private fun String.stringToBytes() = this.toByteArray()
}