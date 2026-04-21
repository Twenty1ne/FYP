import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.axes_grid1.inset_locator import inset_axes

filename = "MBSOGM_III_mass_history.txt"
data = np.loadtxt(filename)

## columns from datafile
timestep     = data[:, 0]
cold_gas     = data[:, 1]
low_mass     = data[:, 2]
high_mass    = data[:, 3]
remnants     = data[:, 4]
heated_gas   = data[:, 5]
total_mass   = data[:, 6]

## scaling mass to avoid massive numbers on the plot axis
scale = 1e6

cold_gas_s = cold_gas / scale
low_mass_s = low_mass / scale
high_mass_s = high_mass / scale
remnants_s = remnants / scale
heated_gas_s = heated_gas / scale
total_mass_s = total_mass / scale

fig, ax = plt.subplots(figsize=(12, 7))

ax.ticklabel_format(style='plain', axis='y')
ax.get_yaxis().get_major_formatter().set_useOffset(False)

## main curves (can curves of interest by commenting out certain curves)
ax.plot(timestep, high_mass_s, label="High-Mass Stars")
ax.plot(timestep, remnants_s, label="SN Remnants")
ax.plot(timestep, cold_gas_s, label="Cold Gas")
ax.plot(timestep, low_mass_s, label="Low-Mass Stars")
ax.plot(timestep, heated_gas_s, label="Heated Gas")
ax.plot(timestep, total_mass_s, label="Total Mass", linewidth=3, color="black")

ax.set_xlabel("Timesteps (Myr)", fontsize = 14)
ax.set_ylabel("Mass ($\\times 10^6$) ($M\\odot$)", fontsize = 14)
ax.set_title("Galaxy Mass Components Over Time", fontsize = 16, fontweight = "bold")
ax.grid(True)
ax.legend(loc="upper right")

## subplot (comment this section out up until and excluding plt.tight_layout() to remove the subplot)
ax_inset = inset_axes(
    ax,
    width="50%",
    height="50%",
    loc="center left",
    bbox_to_anchor=(0.45, -0.02, 1, 1),
    bbox_transform=ax.transAxes,
    borderpad=2
)

ax_inset.plot(timestep, high_mass_s,  label="High-Mass Stars")
ax_inset.plot(timestep, remnants_s,   label="SN Remnants")

ax_inset.set_title("Zoomed In: High-Mass Stars & Remnants", fontsize=12)
ax_inset.grid(True)

ymin = min(np.min(high_mass_s), np.min(remnants_s))
ymax = max(np.max(high_mass_s), np.max(remnants_s))
ax_inset.set_ylim(ymin * 0.9, ymax * 1.1)

ax_inset.legend(loc="upper right", fontsize=8, frameon=True)

plt.tight_layout()
plt.show()
