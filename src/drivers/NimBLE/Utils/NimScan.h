# pragma once

# include "host/ble_gap.h"

class NimScan {
	public:
		virtual void onScanResult(const ble_gap_disc_desc* scanResult) = 0;
};