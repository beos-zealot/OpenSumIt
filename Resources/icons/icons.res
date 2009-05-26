resource('MICN', 400, "accept button") {
	read("OK.data")
}

resource('MICN', 401, "reject button") {
	read("Cancel.data")
}

resource('MICN', 402, "accept button pushed") {
	read("OK_p.data")
}

resource('MICN', 403, "reject button pushed") {
	read("Cancel_p.data")
}

