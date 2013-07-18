RELEASE_FLAGS = "-O3 -Wall -DNDEBUG"
DEBUG_FLAGS = "-O0 -g -Wall -Werror"
RELEASE_PROFILE_FLAGS = "-O3 -Wall -DNDEBUG -DPROFILE -lprofiler"
RELEASE_PGO_FLAGS_GEN = "-O3 -Wall -DNDEBUG -fprofile-generate"
RELEASE_PGO_FLAGS_USE = "-O3 -Wall -DNDEBUG -fprofile-use"

all: clean debug release pricer-smoketests

lib/$(VERSION)/ErrorSummary.o : src/ErrorSummary.cpp
	g++ -std=c++11 -c $< -pipe $(FLAGS) -o $@

lib/$(VERSION)/FeedHandler.o : src/FeedHandler.cpp
	g++ -std=c++11 -c $< -pipe $(FLAGS) -o $@

lib/$(VERSION)/Main.o : src/Main.cpp
	g++ -std=c++11 -c $< -pipe $(FLAGS) -o $@

lib/$(VERSION)/Order.o : src/Order.cpp
	g++ -std=c++11 -c $< -pipe $(FLAGS) -o $@

lib/$(VERSION)/OrderBook.o : src/OrderBook.cpp
	g++ -std=c++11 -c $< -pipe $(FLAGS) -o $@

lib/$(VERSION)/OrderList.o : src/OrderList.cpp
	g++ -std=c++11 -c $< -pipe $(FLAGS) -o $@

lib/$(VERSION)/Tests.o : src/Tests.cpp
	g++ -std=c++11 -c $< -pipe $(FLAGS) -o $@

release-pgo: pricer.in
	mkdir lib;mkdir lib/release;/bin/true
	VERSION=release FLAGS=$(RELEASE_PGO_FLAGS_GEN) LINK_FLAGS="-fprofile-generate -lgcov" make pricer
	time cat pricer.in | ./pricer 200 >/dev/null; /bin/true
	rm pricer
	rm lib/*/*.o
	VERSION=release FLAGS=$(RELEASE_PGO_FLAGS_USE) LINK_FLAGS="-fprofile-use" make pricer
	strip pricer
	time cat pricer.in | ./pricer 200 >/dev/null

release:
	mkdir lib;mkdir lib/release;/bin/true
	VERSION=release FLAGS=$(RELEASE_FLAGS) make pricer
	# Every little helps .. ( runtime performance, this will make debugging much harder )
	strip pricer

debug:
	mkdir lib;mkdir lib/debug;/bin/true
	VERSION=debug FLAGS=$(DEBUG_FLAGS) make pricer-valgrind
	VERSION=debug FLAGS=$(DEBUG_FLAGS) make tests-valgrind

profile:
	mkdir lib;mkdir lib/profile;/bin/true
	VERSION=profile FLAGS=$(RELEASE_PROFILE_FLAGS) make tests-profile
	CPUPROFILE_FREQUENCY=1000 CPUPROFILE=tests.prof ./tests
	VERSION=profile FLAGS=$(RELEASE_PROFILE_FLAGS) make pricer
	strip pricer
	google-pprof --text ./tests ./tests.prof

style:
	# This is my coding standard. There are many like it, but this is mine
	astyle --indent=force-tab --pad-oper --pad-paren --delete-empty-lines --suffix=none --indent-namespaces --indent-col1-comments -n --recursive *.cpp *.hpp

tests: lib/$(VERSION)/ErrorSummary.o lib/$(VERSION)/FeedHandler.o lib/$(VERSION)/Order.o lib/$(VERSION)/OrderBook.o lib/$(VERSION)/OrderList.o lib/$(VERSION)/Tests.o 
	g++ $^ -lboost_unit_test_framework -o tests
	./tests

tests-profile: lib/$(VERSION)/ErrorSummary.o lib/$(VERSION)/FeedHandler.o lib/$(VERSION)/Order.o lib/$(VERSION)/OrderBook.o lib/$(VERSION)/OrderList.o lib/$(VERSION)/Tests.o -lprofiler
	g++ $^ -lboost_unit_test_framework -o tests

tests-valgrind: tests
	valgrind --error-exitcode=1 ./tests
	
pricer.in:
	wget http://www.rgmadvisors.com/problems/orderbook/pricer.in.gz -O - | gunzip > pricer.in
	
pricer.out.1:
	wget http://www.rgmadvisors.com/problems/orderbook/pricer.out.1.gz  -O - | gunzip > pricer.out.1

pricer.out.200:
	wget http://www.rgmadvisors.com/problems/orderbook/pricer.out.200.gz  -O - | gunzip > pricer.out.200

pricer.out.10000:
	wget http://www.rgmadvisors.com/problems/orderbook/pricer.out.10000.gz  -O - | gunzip > pricer.out.10000
	
pricer: lib/$(VERSION)/ErrorSummary.o lib/$(VERSION)/FeedHandler.o lib/$(VERSION)/Main.o lib/$(VERSION)/Order.o lib/$(VERSION)/OrderBook.o lib/$(VERSION)/OrderList.o
	g++ $(LINK_FLAGS) $^ -o pricer -pipe
	
pricer-valgrind: pricer pricer.in
	head -n1000 pricer.in | valgrind --error-exitcode=1 ./pricer 200; /bin/true

pricer-smoketests: pricer.in pricer.out.1 pricer.out.200 pricer.out.10000
	make release
	cat pricer.in | ./pricer 1 > my.pricer.out.1; /bin/true
	cat pricer.in | ./pricer 200 > my.pricer.out.200; /bin/true	
	cat pricer.in | ./pricer 10000 > my.pricer.out.10000; /bin/true
	diff -q pricer.out.1 my.pricer.out.1
	diff -q pricer.out.200 my.pricer.out.200
	diff -q pricer.out.10000 my.pricer.out.10000
	
clean:
	rm -Rf lib tests main pricer lib/*/*.o orderbook_michiel_van_slobbe.tgz tests.prof src/*~ src/*.orig *pricer.out* *~ pricer.in
	
package: clean style debug release
	find . -name "*~" -exec rm {} \;
	rm -Rf tests main lib/* orderbook_michiel_van_slobbe_1.0.tgz
	tar cvzf orderbook_michiel_van_slobbe_1.0.tgz src Makefile README.md 
	