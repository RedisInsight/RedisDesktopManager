##### *Issue:* #####
---
#### Application looks corrupted on my 1080px screen on Linux (too small font and/or broken dialogs) ####


##### *Workaround:* #####
---
#### Run RDM from terminal without Qt Autoscaling: ####
`Exec=env QT_AUTO_SCREEN_SCALE_FACTOR=0 redis-desktop-manager.rdm`
