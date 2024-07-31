### Basic controls
#### Notable shortcuts
- View related options: F1 (filter), F2 (settings), F3 (active thresholds)
- Zoom out: Space
- Zoom in: 
   - double click (onto indicators)
   - CTRL+scroll
- Move left/right: Shift+scroll
- Search: CTRL+S
#### Notable interactions
- Making a selection: 
   - Click, hold the button and drag (overview) to the left or right to select the required section
   - Input the start and / or end time in the fields at the bottom of the view and press ENTER
- Multi-threading view: +/- icons near ranknames
- Flamegraph view: right click (context menu) onto ranknames
- Toggle P2P (in and out): right click (context menu) onto ranknames
- Set custom colors: details pane (per default to the right)

---

### Views
#### Central view (Selection)
Each **row** represents an **MPI rank**; when the thread view is expanded, multiple rows can represent a single MPI rank, with one thread per row. The mode of view is indicated by +/- icons:
- A + means the row can be expanded into the thread view.
- A - means the row has been expanded into the thread view.
- A grayed-out + indicates that no multi-threading was detected.

Within and between the rows, there are **various indicators**: (1) rectangular, colored blocks, (2) arrows, and (3) blue frames.
	
The rectangular blocks within the rows represent **function calls** or recordings of entered regions (often abbreviated as REG). Different functions are assigned different colors:
- MPI functions are usually yellowish-green.
- OpenMPI functions are usually red-orange.
- Other functions are assigned arbitrary colors.

The arrows describe **point-to-point communications** (often abbreviated as P2P) between the ranks or threads, generally between rows.

The blue, rectangular frames that span the rows describe **collective communications** (often abbreviated as CCM); ranks involved are highlighted by hatched function calls within these frames.

#### Overview
Directly below the upper menu bar is the overview. Here, one can always see the entire trace and also the current selection for the main view.

Differences from the main view include that only functions/regions are displayed here, and docking behavior: This element can be detached from and reattached to the main window.

#### Flamegraph
Through a context menu regarding the rank names, one has the option to open the flamegraph view, for a particular rank. In this view, similar to the overview, only functions/regions are displayed; however, they are not overlapping but rather stacked according to their runtime into call hierarchies. 

This view is always synchronized with the central view or the current selection.

---

### Colors
#### At start
Colors are assigned in static steps via RGB-coordinates, there are 159 valid positions between (0|0|0) and (255|255|255) which are assigned in the same order as the functions are encountered (first come, first serve). Encountered functions past these 159 positions will all be assigned white. 
#### During Search
During a search, functions are assigned gray, except the one being searched for, which is displayed in purple.

---

