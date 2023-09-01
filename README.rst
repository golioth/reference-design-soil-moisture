Golioth Soil Moisture Monitor Reference Design
##############################################

Overview
********

This is a Reference Design for a Soil Moisture Monitor that monitors
environmental factors like light intensity, temperature, humidity, and pressure
and uses a relay to actuate grow lights and ventilation.

Local set up
************

Do not clone this repo using git. Zephyr's ``west`` meta tool should be used to
set up your local workspace.

Install the Python virtual environment (recommended)
====================================================

.. code-block:: console

   cd ~
   mkdir golioth-reference-design-soil-moisture
   python -m venv golioth-reference-design-soil-moisture/.venv
   source golioth-reference-design-soil-moisture/.venv/bin/activate
   pip install wheel west

Use west to initialize and install
==================================

.. code-block:: console

   cd ~/golioth-reference-design-soil-moisture
   west init -m git@github.com:golioth/reference-design-soil-moisture.git .
   west update
   west zephyr-export
   pip install -r deps/zephyr/scripts/requirements.txt

Building the application
************************

Build Zephyr sample application for Sparkfun Thing Plus nRF9160 from the top
level of your project. After a successful build you will see a new `build`
directory. Note that any changes (and git commmits) to the project itself will
be inside the `app` folder. The `build` and `deps` directories being one level
higher prevents the repo from cataloging all of the changes to the dependencies
and the build (so no .gitignor is needed)

During building Replace <your.semantic.version> to utilize the DFU functionality
on this Reference Design.

.. code-block:: console

   $ (.venv) west build -b aludel_mini_v1_sparkfun9160_ns app -- -DCONFIG_MCUBOOT_IMAGE_VERSION=\"<your.semantic.version>\"
   $ (.venv) west flash

Configure PSK-ID and PSK using the device shell based on your Golioth
credentials and reboot:

.. code-block:: console

   uart:~$ settings set golioth/psk-id <my-psk-id@my-project>
   uart:~$ settings set golioth/psk <my-psk>
   uart:~$ kernel reboot cold

Golioth Features
****************

This app currently implements Over-the-Air (OTA) firmware updates, Settings
Service, Logging, RPC, and both LightDB State and LightDB Stream data.

Golioth Settings
****************

Settings Service
================

The following settings should be set in the Device Settings menu of the
`Golioth Console`_.

``LOOP_DELAY_S``
   Adjusts the delay between sensor readings. Set to an integer value (seconds).

   Default value is ``60`` seconds.

``MOISTURE_LEVEL_X``
   Determines threshhold values for the moisture sensor. Set to an integer value corresponding to 'counts'. 

   * ``MOISTURE_LEVEL_0``: ``3400`` (default value)
   * ``MOISTURE_LEVEL_20``: ``3200`` (default value)
   * ``MOISTURE_LEVEL_40``: ``3000`` (default value)
   * ``MOISTURE_LEVEL_60``: ``2800`` (default value)
   * ``MOISTURE_LEVEL_80``: ``2600`` (default value)

Remote Procedure Call (RPC) Service
===================================

The following RPCs can be initiated in the Remote Procedure Call menu of the
`Golioth Console`_.

``get_network_info``
   Query and return network information.

``reboot``
   Reboot the system.

``set_log_level``
   Set the log level.

   The method takes a single parameter which can be one of the following integer
   values:

   * ``0``: ``LOG_LEVEL_NONE``
   * ``1``: ``LOG_LEVEL_ERR``
   * ``2``: ``LOG_LEVEL_WRN``
   * ``3``: ``LOG_LEVEL_INF``
   * ``4``: ``LOG_LEVEL_DBG``

LightDB State and LightDB Stream data
=====================================

Time-Series Data (LightDB Stream)
---------------------------------

Sensor data is periodically sent to the following ``sensor/*`` endpoints of the
LightDB Stream service:

* ``sensor/imu/accel_x``: Acceleration X-axis (m/s²)
* ``sensor/imu/accel_y``: Acceleration Y-axis (m/s²)
* ``sensor/imu/accel_z``: Acceleration Z-axis (m/s²)
* ``sensor/ligth/b``: Blue Light Value
* ``sensor/ligth/g``: Green Light Value
* ``sensor/ligth/int``: Clear Light Intensity (LUX)
* ``sensor/ligth/r``: Red Light Value
* ``sensor/moisture/level``: Mosture Level
* ``sensor/moisture/raw``: Moisture Reading RAW value
* ``sensor/weather/humidity``:Humidity (%RH)
* ``sensor/weather/pressure``: Pressure (kPa)
* ``sensor/weather/temp``: Temperature (°C)

Battery voltage and level readings are periodically sent to the following
``battery/*`` endpoints:

* ``battery/batt_v``: Battery Voltage (V)
* ``battery/batt_lvl``: Battery Level (%)

Stateful Data (LightDB State)
-----------------------------

The concept of Digital Twin is demonstrated with the LightDB State
``example_int0`` and ``example_int1`` variables that are members of the ``desired``
and ``state`` endpoints.

* ``desired`` values may be changed from the cloud side. The device will recognize
  these, validate them for [0..65535] bounding, and then reset these endpoints
  to ``-1``

* ``state`` values will be updated by the device whenever a valid value is
  received from the ``desired`` endpoints. The cloud may read the ``state``
  endpoints to determine device status, but only the device should ever write to
  the ``state`` endpoints.


Hardware Variations
*******************

Nordic nRF9160 DK
=================

This reference design may be built for the `Nordic nRF9160 DK`_, with the
`MikroE Arduino UNO click shield`_ to interface the two click boards.

* Position the `MikroE Weather Click`_ board in Slot 1
* Position the `MikroE Shuttle Click`_ board in Slot 2

Connect the `MikroE Hydro Probe Click`_, the `MikroE IR Gesture Click`_ and the
`LIS2DH Adapter Board`_ to the `MikroE Shuttle Click`_.

Use the following commands to build and program. (Use the same console commands
from above to provision this board after programming the firmware.)

.. code-block:: text

   $ (.venv) west build -p -b nrf9160dk_nrf9160_ns app -- -DCONFIG_MCUBOOT_IMAGE_VERSION=\"<your.semantic.version>\"
   $ (.venv) west flash

External Libraries
******************

The following code libraries are installed by default. If you are not using the
custom hardware to which they apply, you can safely remove these repositories
from ``west.yml`` and remove the includes/function calls from the C code.

* `golioth-zephyr-boards`_ includes the board definitions for the Golioth
  Aludel-Mini
* `libostentus`_ is a helper library for controlling the Ostentus ePaper
  faceplate

Pulling in updates from the Reference Design Template
*****************************************************

This reference design was forked from the `Reference Design Template`_ repo. We
recommend the following workflow to pull in future changes:

* Setup

  * Create a ``template`` remote based on the Reference Design Template repository

* Merge in template changes

  * Fetch template changes and tags
  * Merge template release tag into your ``main`` (or other branch)
  * Resolve merge conflicts (if any) and commit to your repository

.. code-block:: shell

   # Setup
   git remote add template https://github.com/golioth/reference-design-template.git
   git fetch template --tags

   # Merge in template changes
   git fetch template --tags
   git checkout your_local_branch
   git merge template_v1.0.0

   # Resolve merge conflicts if necessary
   git add resolved_files
   git commit


.. _Golioth Console: https://console.golioth.io
.. _Nordic nRF9160 DK: https://www.nordicsemi.com/Products/Development-hardware/nrf9160-dk
.. _golioth-zephyr-boards: https://github.com/golioth/golioth-zephyr-boards
.. _libostentus: https://github.com/golioth/libostentus
.. _MikroE Arduino UNO click shield: https://www.mikroe.com/arduino-uno-click-shield
.. _MikroE Weather Click: https://www.mikroe.com/weather-click
.. _MikroE Shuttle Click: https://www.mikroe.com/shuttle-click
.. _MikroE Hydro Probe Click: https://www.mikroe.com/hydro-probe-click
.. _MikroE IR Gesture Click: https://www.mikroe.com/ir-gesture-click
.. _LIS2DH Adapter Board: https://www.st.com/en/evaluation-tools/steval-mki135v1.html
.. _Reference Design Template: https://github.com/golioth/reference-design-template
