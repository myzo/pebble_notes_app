Pebble.addEventListener('ready',
  function (e) {
    console.log('Ready');
  }
);

Pebble.addEventListener('showConfiguration',
  function (e) {
    Pebble.openURL('http://myzo.github.io/pebble_notes_app');
  }
);

Pebble.addEventListener('webviewclosed',
  function (e) {
    var configuration = JSON.parse(decodeURIComponent(e.response));
    Pebble.sendAppMessage(
      {
        "KEY_NOTES": configuration.notes,
        "KEY_LENGTH": configuration.notesLength
      },
      function (e) {
        console.log("Sending configuration data...");
      },
      function (e) {
        console.log("Settings feedback failed! " + e);
      }
    );
  }
);