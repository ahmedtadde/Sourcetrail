#include <cxxtest/TestSuite.h>

#include "settings/Settings.h"
#include "settings/migration/SettingsMigrator.h"
#include "settings/migration/SettingsMigrationLambda.h"
#include "settings/migration/SettingsMigrationMoveKey.h"
#include "utility/text/TextAccess.h"

class SettingsMigratorTestSuite : public CxxTest::TestSuite
{
public:
	void test_migrator_changes_nothing_without_migrations_except_version()
	{
		TestSettings settingsBefore = createSettings(
			"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
			"<config>\n"
			"    <value>2</value>\n"
			"</config>\n"
		);

		TestSettings settingsAfter = createSettings(
			"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
			"<config>\n"
			"    <value>2</value>\n"
			"    <version>1</version>\n"
			"</config>\n"
		);

		SettingsMigrator migrator;
		migrator.migrate(&settingsBefore, 1);

		TS_ASSERT_EQUALS(settingsBefore.getAsText(), settingsAfter.getAsText());
	}

	void test_migrator_changes_name()
	{
		TestSettings settingsBefore = createSettings(
			"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
			"<config>\n"
			"    <value>2</value>\n"
			"</config>\n"
		);

		TestSettings settingsAfter = createSettings(
			"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
			"<config>\n"
			"    <int>2</int>\n"
			"    <version>1</version>\n"
			"</config>\n"
		);

		SettingsMigrator migrator;
		migrator.addMigration(1, std::make_shared<SettingsMigrationMoveKey>("value", "int"));
		migrator.migrate(&settingsBefore, 1);

		TS_ASSERT_EQUALS(settingsBefore.getAsText(), settingsAfter.getAsText());
	}

	void test_migrator_changes_path()
	{
		TestSettings settingsBefore = createSettings(
			"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
			"<config>\n"
			"    <value>2</value>\n"
			"</config>\n"
		);

		TestSettings settingsAfter = createSettings(
			"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
			"<config>\n"
			"    <sub>\n"
			"        <int>2</int>\n"
			"    </sub>\n"
			"    <version>1</version>\n"
			"</config>\n"
		);

		SettingsMigrator migrator;
		migrator.addMigration(1, std::make_shared<SettingsMigrationMoveKey>("value", "sub/int"));
		migrator.migrate(&settingsBefore, 1);

		TS_ASSERT_EQUALS(settingsBefore.getAsText(), settingsAfter.getAsText());
	}

	void test_migrator_changes_group_name()
	{
		TestSettings settingsBefore = createSettings(
			"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
			"<config>\n"
			"    <values>\n"
			"        <value>2</value>\n"
			"        <value>3</value>\n"
			"        <value>4</value>\n"
			"    </values>\n"
			"</config>\n"
		);

		TestSettings settingsAfter = createSettings(
			"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
			"<config>\n"
			"    <vals>\n"
			"        <value>2</value>\n"
			"        <value>3</value>\n"
			"        <value>4</value>\n"
			"    </vals>\n"
			"    <version>1</version>\n"
			"</config>\n"
		);

		SettingsMigrator migrator;
		migrator.addMigration(1, std::make_shared<SettingsMigrationMoveKey>("values/value", "vals/value"));
		migrator.migrate(&settingsBefore, 1);

		TS_ASSERT_EQUALS(settingsBefore.getAsText(), settingsAfter.getAsText());
	}

	void test_migrator_changes_group_element_name()
	{
		TestSettings settingsBefore = createSettings(
			"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
			"<config>\n"
			"    <values>\n"
			"        <value>2</value>\n"
			"        <value>3</value>\n"
			"        <value>4</value>\n"
			"    </values>\n"
			"</config>\n"
		);

		TestSettings settingsAfter = createSettings(
			"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
			"<config>\n"
			"    <values>\n"
			"        <val>2</val>\n"
			"        <val>3</val>\n"
			"        <val>4</val>\n"
			"    </values>\n"
			"    <version>1</version>\n"
			"</config>\n"
		);

		SettingsMigrator migrator;
		migrator.addMigration(1, std::make_shared<SettingsMigrationMoveKey>("values/value", "values/val"));
		migrator.migrate(&settingsBefore, 1);

		TS_ASSERT_EQUALS(settingsBefore.getAsText(), settingsAfter.getAsText());
	}

	void test_migrator_changes_only_up_specified_version()
	{
		TestSettings settingsBefore = createSettings(
			"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
			"<config>\n"
			"    <value>2</value>\n"
			"</config>\n"
		);

		TestSettings settingsAfter = createSettings(
			"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
			"<config>\n"
			"    <int>2</int>\n"
			"    <version>1</version>\n"
			"</config>\n"
		);

		SettingsMigrator migrator;
		migrator.addMigration(1, std::make_shared<SettingsMigrationMoveKey>("value", "int"));
		migrator.addMigration(2, std::make_shared<SettingsMigrationMoveKey>("int", "val"));
		migrator.migrate(&settingsBefore, 1);

		TS_ASSERT_EQUALS(settingsBefore.getAsText(), settingsAfter.getAsText());
	}

	void test_migrator_changes_only_from_specified_version()
	{
		TestSettings settingsBefore = createSettings(
			"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
			"<config>\n"
			"    <int>2</int>\n"
			"    <version>1</version>\n"
			"</config>\n"
		);

		TestSettings settingsAfter = createSettings(
			"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
			"<config>\n"
			"    <val>2</val>\n"
			"    <version>2</version>\n"
			"</config>\n"
		);

		SettingsMigrator migrator;
		migrator.addMigration(1, std::make_shared<SettingsMigrationMoveKey>("value", "int"));
		migrator.addMigration(2, std::make_shared<SettingsMigrationMoveKey>("int", "val"));
		migrator.migrate(&settingsBefore, 2);

		TS_ASSERT_EQUALS(settingsBefore.getAsText(), settingsAfter.getAsText());
	}

	void test_migrator_changes_for_multiple_versions()
	{
		TestSettings settingsBefore = createSettings(
			"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
			"<config>\n"
			"    <value>2</value>\n"
			"</config>\n"
		);

		TestSettings settingsAfter = createSettings(
			"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
			"<config>\n"
			"    <val>2</val>\n"
			"    <version>2</version>\n"
			"</config>\n"
		);

		SettingsMigrator migrator;
		migrator.addMigration(1, std::make_shared<SettingsMigrationMoveKey>("value", "int"));
		migrator.addMigration(2, std::make_shared<SettingsMigrationMoveKey>("int", "val"));
		migrator.migrate(&settingsBefore, 2);

		TS_ASSERT_EQUALS(settingsBefore.getAsText(), settingsAfter.getAsText());
	}

	void test_migrator_changes_for_multiple_migrations()
	{
		TestSettings settingsBefore = createSettings(
			"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
			"<config>\n"
			"    <value>2</value>\n"
			"    <element>hi there</element>\n"
			"</config>\n"
		);

		TestSettings settingsAfter = createSettings(
			"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
			"<config>\n"
			"    <val>2</val>\n"
			"    <ele>hi there</ele>\n"
			"    <version>2</version>\n"
			"</config>\n"
		);

		SettingsMigrator migrator;
		migrator.addMigration(1, std::make_shared<SettingsMigrationMoveKey>("value", "val"));
		migrator.addMigration(1, std::make_shared<SettingsMigrationMoveKey>("element", "ele"));
		migrator.migrate(&settingsBefore, 2);

		TS_ASSERT_EQUALS(settingsBefore.getAsText(), settingsAfter.getAsText());
	}

	void test_migrator_changes_for_multiple_versions_and_migrations()
	{
		TestSettings settingsBefore = createSettings(
			"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
			"<config>\n"
			"    <value>2</value>\n"
			"    <element>two</element>\n"
			"    <element>three</element>\n"
			"    <element>four</element>\n"
			"</config>\n"
		);

		TestSettings settingsAfter = createSettings(
			"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
			"<config>\n"
			"    <int>2</int>\n"
			"    <elements>\n"
			"        <element>two</element>\n"
			"        <element>three</element>\n"
			"        <element>four</element>\n"
			"    </elements>\n"
			"    <version>3</version>\n"
			"</config>\n"
		);

		SettingsMigrator migrator;
		migrator.addMigration(1, std::make_shared<SettingsMigrationMoveKey>("value", "int/val"));
		migrator.addMigration(1, std::make_shared<SettingsMigrationMoveKey>("element", "ele"));
		migrator.addMigration(2, std::make_shared<SettingsMigrationMoveKey>("int/val", "int"));
		migrator.addMigration(3, std::make_shared<SettingsMigrationMoveKey>("ele", "elements/element"));
		migrator.migrate(&settingsBefore, 3);

		TS_ASSERT_EQUALS(settingsBefore.getAsText(), settingsAfter.getAsText());
	}

	void test_migrator_with_lambda()
	{
		TestSettings settingsBefore = createSettings(
			"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
			"<config>\n"
			"    <value>2</value>\n"
			"</config>\n"
		);

		TestSettings settingsAfter = createSettings(
			"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
			"<config>\n"
			"    <value>4</value>\n"
			"    <version>1</version>\n"
			"</config>\n"
		);

		SettingsMigrator migrator;
		migrator.addMigration(
			1, std::make_shared<SettingsMigrationLambda>(
			[](const SettingsMigration* migration, Settings* settings)
			{
				migration->setValueInSettings<int>(settings, "value", migration->getValueFromSettings<int>(settings, "value", 0) * 2);
			}
		));
		migrator.migrate(&settingsBefore, 1);

		TS_ASSERT_EQUALS(settingsBefore.getAsText(), settingsAfter.getAsText());
	}

	void test_migrator_with_multiple_lambdas()
	{
		TestSettings settingsBefore = createSettings(
			"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
			"<config>\n"
			"    <value>2</value>\n"
			"</config>\n"
		);

		TestSettings settingsAfter = createSettings(
			"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
			"<config>\n"
			"    <value>3</value>\n"
			"    <version>2</version>\n"
			"</config>\n"
		);

		SettingsMigrator migrator;
		migrator.addMigration(
			1, std::make_shared<SettingsMigrationLambda>(
				[](const SettingsMigration* migration, Settings* settings)
				{
					migration->setValueInSettings<int>(settings, "value", migration->getValueFromSettings<int>(settings, "value", 0) * 2);
				}
			)
		);
		migrator.addMigration(
			2, std::make_shared<SettingsMigrationLambda>(
				[](const SettingsMigration* migration, Settings* settings)
				{
					migration->setValueInSettings<int>(settings, "value", migration->getValueFromSettings<int>(settings, "value", 0) - 1);
				}
			)
		);
		migrator.migrate(&settingsBefore, 2);

		TS_ASSERT_EQUALS(settingsBefore.getAsText(), settingsAfter.getAsText());
	}

private:
	class TestSettings
		: public Settings
	{
	public:
		static TestSettings createFromText(const std::shared_ptr<TextAccess> textAccess)
		{
			TestSettings settings;
			settings.m_config = ConfigManager::createAndLoad(textAccess);
			return settings;
		}

		std::string getAsText() const
		{
			if (m_config)
			{
				return m_config->toString();
			}

			return "";
		}
	};

	TestSettings createSettings(const std::string& text)
	{
		return TestSettings::createFromText(TextAccess::createFromString(text));
	}
};
