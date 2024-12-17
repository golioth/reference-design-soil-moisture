..
   Copyright (c) 2022-2023 Golioth, Inc.
   SPDX-License-Identifier: Apache-2.0

Golioth Soil Moisture Monitor Reference Design
##############################################

Overview
********

This is a Reference Design for a Soil Moisture Monitor that monitors
environmental factors like light intensity, temperature, humidity, and pressure
and uses a relay to actuate grow lights and ventilation.

Local set up
************

.. pull-quote::
   [!IMPORTANT]

   Do not clone this repo using git. Zephyr's ``west`` meta tool should be used to
   set up your local workspace.

Install the Python virtual environment (recommended)
====================================================

.. code-block:: shell

   cd ~
   mkdir golioth-reference-design-soil-moisture
   python -m venv golioth-reference-design-soil-moisture/.venv
   source golioth-reference-design-soil-moisture/.venv/bin/activate
   pip install wheel west

Use west to initialize and install
==================================

.. code-block:: shell

   cd ~/golioth-reference-design-soil-moisture
   west init -m git@github.com:golioth/reference-design-soil-moisture.git .
   west update
   west zephyr-export
   pip install -r deps/zephyr/scripts/requirements.txt

Building the application
************************

Build the Zephyr sample application for the `Nordic nRF9160 DK`_
(``nrf9160dk_nrf9160_ns``) from the top level of your project. After a
successful build you will see a new ``build`` directory. Note that any changes
(and git commits) to the project itself will be inside the ``app`` folder. The
``build`` and ``deps`` directories being one level higher prevents the repo from
cataloging all of the changes to the dependencies and the build (so no
``.gitignore`` is needed).

Prior to building, update ``VERSION`` file to reflect the firmware version number you want to assign
to this build. Then run the following commands to build and program the firmware onto the device.


.. pull-quote::
   [!IMPORTANT]

   You must perform a pristine build (use ``-p`` or remove the ``build`` directory)
   after changing the firmware version number in the ``VERSION`` file for the change to take effect.

.. code-block:: text

   $ (.venv) west build -p -b nrf9160dk/nrf9160/ns --sysbuild app
   $ (.venv) west flash

Configure PSK-ID and PSK using the device shell based on your Golioth
credentials and reboot:

.. code-block:: text

   uart:~$ settings set golioth/psk-id <my-psk-id@my-project>
   uart:~$ settings set golioth/psk <my-psk>
   uart:~$ kernel reboot cold

Add Pipeline to Golioth
***********************

Golioth uses `Pipelines`_ to route stream data. This gives you flexibility to change your data
routing without requiring updated device firmware.

Whenever sending stream data, you must enable a pipeline in your Golioth project to configure how
that data is handled. Add the contents of ``pipelines/cbor-to-lightdb.yml`` as a new pipeline as
follows (note that this is the default pipeline for new projects and may already be present):

   1. Navigate to your project on the Golioth web console.
   2. Select ``Pipelines`` from the left sidebar and click the ``Create`` button.
   3. Give your new pipeline a name and paste the pipeline configuration into the editor.
   4. Click the toggle in the bottom right to enable the pipeline and then click ``Create``.

All data streamed to Golioth in CBOR format will now be routed to LightDB Stream and may be viewed
using the web console. You may change this behavior at any time without updating firmware simply by
editing this pipeline entry.

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

This reference design may be built for a variety of different boards.

Prior to building, update ``VERSION`` file to reflect the firmware version number you want to assign
to this build. Then run the following commands to build and program the firmware onto the device.

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

   $ (.venv) west build -p -b nrf9160dk/nrf9160/ns --sysbuild app
   $ (.venv) west flash

Golioth Aludel Mini
===================

This reference design may be built for the Golioth Aludel Mini board.

.. code-block:: text

   $ (.venv) west build -p -b aludel_mini/nrf9160/ns --sysbuild app
   $ (.venv) west flash

Golioth Aludel Elixir
=====================

This reference design may be built for the Golioth Aludel Elixir board. By default this will build
for the latest hardware revision of this board.

.. code-block:: text

   $ (.venv) west build -p -b aludel_elixir/nrf9160/ns --sysbuild app
   $ (.venv) west flash

To build for a specific board revision (e.g. Rev A) add the revision suffix ``@<rev>``.

.. code-block:: text

   $ (.venv) west build -p -b aludel_elixir@A/nrf9160/ns --sysbuild app
   $ (.venv) west flash

OTA Firmware Update
*******************

This application includes the ability to perform Over-the-Air (OTA) firmware updates:

1. Update the version number in the `VERSION` file and perform a pristine (important) build to
   incorporate the version change.
2. Upload the `build/app/zephyr/zephyr.signed.bin` file as an artifact for your Golioth project
   using `main` as the package name.
3. Create and roll out a release based on this artifact.

Visit `the Golioth Docs OTA Firmware Upgrade page`_ for more info.

External Libraries
******************

The following code libraries are installed by default. If you are not using the
custom hardware to which they apply, you can safely remove these repositories
from ``west.yml`` and remove the includes/function calls from the C code.

* `golioth-zephyr-boards`_ includes the board definitions for the Golioth
  Aludel-Mini
* `libostentus`_ is a helper library for controlling the Ostentus ePaper
  faceplate
* `zephyr-network-info`_ is a helper library for querying, formatting, and returning network
  connection information via Zephyr log or Golioth RPC

.. _Golioth Console: https://console.golioth.io
.. _Nordic nRF9160 DK: https://www.nordicsemi.com/Products/Development-hardware/nrf9160-dk
.. _Pipelines: https://docs.golioth.io/data-routing
.. _the Golioth Docs OTA Firmware Upgrade page: https://docs.golioth.io/firmware/golioth-firmware-sdk/firmware-upgrade/firmware-upgrade
.. _golioth-zephyr-boards: https://github.com/golioth/golioth-zephyr-boards
.. _libostentus: https://github.com/golioth/libostentus
.. _zephyr-network-info: https://github.com/golioth/zephyr-network-info
.. _MikroE Arduino UNO click shield: https://www.mikroe.com/arduino-uno-click-shield
.. _MikroE Weather Click: https://www.mikroe.com/weather-click
.. _MikroE Shuttle Click: https://www.mikroe.com/shuttle-click
.. _MikroE Hydro Probe Click: https://www.mikroe.com/hydro-probe-click
.. _MikroE IR Gesture Click: https://www.mikroe.com/ir-gesture-click
.. _LIS2DH Adapter Board: https://www.st.com/en/evaluation-tools/steval-mki135v1.html
.. _Reference Design Template: https://github.com/golioth/reference-design-template
