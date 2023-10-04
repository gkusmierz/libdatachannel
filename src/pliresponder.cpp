/**
 * Copyright (c) 2023 Arda Cinar
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "pliresponder.hpp"

#if RTC_ENABLE_MEDIA

namespace rtc {

ChainedIncomingControlProduct PliResponder::processIncomingControlMessage(message_ptr message) {
	size_t offset = 0;

	while ((sizeof(RtcpHeader) + offset) <= message->size()) {
		auto header = reinterpret_cast<rtc::RtcpHeader*>(message->data() + offset);
		uint8_t payload_type = header->payloadType();

		if (payload_type == 196) { 
			// FIR message, call pli handler anyway
			onPli();
			break;
		} else if (payload_type == 206) {
			// On a payload specific fb message, there is a "feedback message type" (FMT) in the
			// header instead of a report count. PT = 206, FMT = 1 means a PLI message
			uint8_t feedback_message_type = header->reportCount();
			if (feedback_message_type == 1) {
				onPli();
				break;
			}
		}
		offset += header->lengthInBytes();
	}
	return { message, std::nullopt };
}

PliResponder::PliResponder(std::function<void(void)> onPli) : onPli(onPli) { }

}

#endif // RTC_ENABLE_MEDIA
