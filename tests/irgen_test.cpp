TEST_CASE("Gen Test") {
	namespace fs = std::experimental::filesystem;
	std::string acls_path = "../tests/acl/";
	MESSAGE("testing");
	for (auto &p : fs::directory_iterator(acls_path)) {
		// Build file paths
		std::string acl_string_path = p.path().string();
		std::string ir_string_path = acl_string_path;
		MESSAGE("Gen: " << acl_string_path);

		auto i = ir_string_path.find("/acl/");
		if (i != std::string::npos)
			ir_string_path.replace(i, 5, "/ir/");

		ir_string_path.replace(ir_string_path.size()-4, 4, ".ir");

		fs::path acl_path = fs::path(acl_string_path);
		fs::path ir_path = fs::path(ir_string_path);

		// Check both source and ir files exsist
		if(!fs::exists(acl_path) || !fs::exists(ir_path)) 
			continue;
	
		// read files
		std::ifstream acl_file(acl_path.string());
		std::string acl((std::istreambuf_iterator<char>(acl_file)),
			std::istreambuf_iterator<char>());

		std::ifstream ir_file(ir_path.string());
		std::string ir((std::istreambuf_iterator<char>(ir_file)),
			std::istreambuf_iterator<char>());

		// Check ir
		Irgen irgen = Irgen(acl);
		CHECK_EQ(irgen.to_string(), ir);
	}
}