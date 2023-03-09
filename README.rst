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

   * MOISTURE_LEVEL_0
   * MOISTURE_LEVEL_20
   * MOISTURE_LEVEL_40
   * MOISTURE_LEVEL_60
   * MOISTURE_LEVEL_80

Remote Procedure Call (RPC) Service
===================================

The following RPCs can be initiated in the Remote Procedure Call menu of the
`Golioth Console`_.

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

``clean_pm_sensor``
   Initiate the SPS30 particulate matter fan-cleaning procedure manually. The
   fan cleaning procedure takes approximately 10s to complete.


.. _Golioth Console: https://console.golioth.io
