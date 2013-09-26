
all-sub: $(INTDIR) all

clean-sub: clean

$(INTDIR):
!if "$(SRCDIR)" == ""
	@if not exist $(TARGET) mkdir $(TARGET)
!endif
	@if not exist $(INTDIR) mkdir $(INTDIR)

{$(SUBDIR)}.c{$(INTDIR)}.obj::
	$(CC) -c $(CFLAGS) /Fo"$(INTDIR)\\" $<


