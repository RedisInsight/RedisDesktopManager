#include "ItemWithNaturalSort.h"

ItemWithNaturalSort::ItemWithNaturalSort()
	: QStandardItem()
{

}

ItemWithNaturalSort::ItemWithNaturalSort(const QIcon & icon, const QString & text)
	: QStandardItem(icon, text)
{

}

/**
* Natural sort algorithm ported from 
* http://sourcefrog.net/projects/natsort/
**/

static int
compare_left(QString::iterator a,  QString::iterator b)
{
     /* Compare two left-aligned numbers: the first to have a
        different value wins. */
     for (;; a++, b++) {
		  if (!a->isDigit()  &&  !b->isDigit())
			   return 0;
		  else if (!a->isDigit())
			   return -1;
		  else if (!b->isDigit())
			   return +1;
		  else if (*a < *b)
			   return -1;
		  else if (*a > *b)
			   return +1;
     }
	  
     return 0;
}

static int
compare_right(QString::iterator a,  QString::iterator b)
{
     int bias = 0;
     
     /* The longest run of digits wins.  That aside, the greatest
	value wins, but we can't know that it will until we've scanned
	both numbers to know that they have the same magnitude, so we
	remember it in BIAS. */
     for (;; a++, b++) {
		  if (!a->isDigit()  &&  !b->isDigit())
			   return bias;
		  else if (!a->isDigit())
			   return -1;
		  else if (!b->isDigit())
			   return +1;
		  else if (*a < *b) {
			   if (!bias)
				bias = -1;
		  } else if (*a > *b) {
			   if (!bias)
				bias = +1;
		  } else {
			   return bias;
		  }		
	}

	return 0;
}


bool ItemWithNaturalSort::operator<(const QStandardItem & second) const
{
	QString a = text();
	QString b = second.text();

    int ai, bi;
    QChar ca, cb;
    int fractional, result;
         
    ai = bi = 0;

    while (true) {
		ca = a[ai]; cb = b[bi];

		/* skip over leading spaces or zeros */
		while (ca.isSpace())
			ca = a[++ai];

		while (cb.isSpace())
			cb = b[++bi];

		/* process run of digits */
		if (ca.isDigit()  &&  cb.isDigit()) {
			fractional = (ca == '0' || cb == '0');

			if (fractional) {
				if ((result = compare_left(a.begin()+ai, b.begin()+bi)) != 0)
					return result;
				} else {
					if ((result = compare_right(a.begin()+ai, b.begin()+bi)) != 0)
						return result;
				}
		}

		if (ca.isNull() && cb.isNull()) {
			/* The strings compare the same.  Perhaps the caller
					will want to call strcmp to break the tie. */
			return false;
		}
	  
		if (ca < cb)
			return true;
		else if (ca > cb)
			return false;

		++ai; ++bi;
    }
}